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
  node_t *ptr_16MiB = slab[23].head;
  while(ptr_16MiB < (node_t*)heap.end) {
    ptr_16MiB->isfree = 1;
    ptr_16MiB->size = MIN((uint64_t)1 << 24, (uint64_t)(heap.end - (uintptr_t)ptr_16MiB));
    ptr_16MiB->next = (node_t*)((char*)ptr_16MiB + (1 << 24) + sizeof(node_t));
     printf("a node at %p\n, node size is %u", ptr_16MiB, ptr_16MiB->size);
    ptr_16MiB = ptr_16MiB->next;
  }
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
