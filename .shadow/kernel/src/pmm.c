#include <common.h>
#define INDEX2SIZE(index) (1 << (index + 1))
#define SIZE2INDEX(size) ({    \
  assert((size) != 0);         \
  size_t msize = (size);       \
  int cnt = 0;                 \
  while(msize >> 1) {          \
    cnt++;                     \
    msize >>= 1;               \
  }                            \
  cnt - 1;                     \
})                             \

typedef struct _node_t {
  bool isfree;
  union {
    size_t size;
    struct _node_t *next;
  };
} node_t;

typedef struct {
  int lock;
  node_t *head;
} slab_t;

slab_t slab[24]; // size at index i is SIZE2INDEX(i) - sizeof(node_t)


void list_push_front(node_t **head, node_t *new) {
  new->next = (*head);
  (*head) = new;
}

node_t *list_pop_front(node_t **head) {
  node_t *ret = (*head);
  (*head) = (*head)->next;
  return ret;
}

node_t *list_remove(node_t **head, node_t *target) {
  node_t **cur = head;
  while(*cur != NULL && *cur != target) {
    cur = &((*cur)->next);
  }
  assert(*cur != NULL);
  node_t *next = (*cur)->next;
  (*cur) = next;
  target->next = NULL;
  return target;
}

node_t *node_split(size_t index) {
  return NULL;
}

// if able to merge, return the new node's address, else return itself
node_t *node_merge(node_t *prev) {
  // must first mark prev as free in the caller
  assert(prev->isfree == 1);

  size_t size = prev->size + sizeof(node_t);
  size_t index = SIZE2INDEX(size);
  node_t *buddy = (node_t*)((uintptr_t)(prev) ^ (1 << (index + 1)));
  printf("merge %x node, prev is %p, buddy is %p", size, prev, buddy);
  if(buddy->isfree) {
    // remove buddy from the slab
    list_remove(&slab[index].head, buddy);

    // build the merged node
    node_t *ret = (node_t*)((uintptr_t)prev & (1 << (index + 1)));
    ret->isfree = 1;
    ret->size = size * 2 - sizeof(node_t);

    // insert ret to the slab
    list_push_front(&(slab[index].head), ret);

    printf("node %p is able to merge, return node %p, size %x", prev, ret, ret->size);
    return ret;
  } else {
    return prev;
  }
}

static void *kalloc(size_t size) {
  return NULL;
}

static void kfree(void *ptr) {
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  slab[23].head = heap.start;
  slab[23].lock = 0;
  node_t *ptr = slab[23].head;
  // allocate 16MiB nodes
  while((uintptr_t)ptr + INDEX2SIZE(23) < (uintptr_t)heap.end) {
    ptr->isfree = 1;
    ptr->next = (node_t*)((char*)ptr + INDEX2SIZE(23));
    printf("a 16MiB node at %p\n", ptr);
    ptr = ptr->next;
  }
  // make full use of the rest space
  for(int i = 22; i >= SIZE2INDEX(sizeof(node_t)); i--) {
    slab[i].lock = 0;
    if((uintptr_t)ptr + INDEX2SIZE(i) < (uintptr_t)heap.end) {
      assert(SIZE2INDEX(INDEX2SIZE(i)) == i);
      slab[i].head = ptr;
      printf("now init %x node\n", INDEX2SIZE(i));
      while((uintptr_t)ptr + INDEX2SIZE(i) < (uintptr_t)heap.end) {
        ptr->isfree = 1;
        ptr->next = (node_t*)((char*)ptr + INDEX2SIZE(i));
        printf("\ta node at %p\n", ptr);
        ptr = ptr->next;
      }
    } else {
      printf("cannot init %x node\n", INDEX2SIZE(i));
      slab[i].head = NULL;
    }
  }
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
