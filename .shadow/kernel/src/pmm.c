#include <common.h>

#define MAGIC 0xff // warning: cannot overlap with physical address

static size_t alias(size_t size) {
  if(size == 0) {
    return 0;
  }
  size_t msize = 2;
  while(size > msize) {
    msize <<= 1;
  }
  return msize;
}

typedef struct __node_t {
    int                size;
    union {
      int             magic;
      struct __node_t *next;
    };
} node_t;

static node_t *head;

static void *kalloc(size_t size) {
  size = alias(size);
  return NULL;
}

static void kfree(void *ptr) {
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  head = (node_t*) heap.start;
  head->magic = MAGIC;
  head->size = pmsize - sizeof(node_t);
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
