#include <common.h>

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

static void *kalloc(size_t size) {
  return NULL;
}

static void kfree(void *ptr) {
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  slab[23].head = heap.start;
  node_t *ptr = slab[23].head;
  while((uintptr_t)ptr + (1 << 24) < (uintptr_t)heap.end) {
    ptr->isfree = 1;
    ptr->next = (node_t*)((char*)ptr + (1 << 24) + sizeof(node_t));
    printf("a 16MiB node at %p\n", ptr);
    ptr = ptr->next;
  }
  for(int i = 22; i >= 0; i--) {
    slab[i].head = ptr;
    while((uintptr_t)ptr + (1 << (i + 1)) < (uintptr_t)heap.end) {
      ptr->isfree = 1;
      ptr->next = (node_t*)((char*)ptr + (1 << (i + 1)) + sizeof(node_t));
      printf("a %xBytes node at %p\n", 1 << (i + 1), ptr);
      ptr = ptr->next;
    }
  }
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
