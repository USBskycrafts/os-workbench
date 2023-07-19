#include <common.h>
#define INDEX2SIZE(index) (1 << (index + 1))

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

slab_t slab[24];


void list_push_front(node_t **head, node_t *new) {
  new->next = (*head);
  (*head) = new;
}

node_t *list_pop_front(node_t **head) {
    node_t *ret = (*head);
    (*head) = (*head)->next;
    return ret;
}

node_t *node_split(size_t index) {
  return NULL;
}

node_t *node_merge(node_t *prev) {
  return NULL;
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
  for(int i = 22; i >= 0; i--) {
    slab[i].lock = 0;
    if((uintptr_t)ptr + INDEX2SIZE(i) < (uintptr_t)heap.end) {
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
