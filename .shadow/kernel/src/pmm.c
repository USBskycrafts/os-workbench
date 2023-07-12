#include <common.h>

#define MAXCPU 8
#define cpuid cpu_current() % MAXCPU

typedef struct __node_t {
  size_t size;
  struct __node_t *prev;
  struct __node_t *next;
} node_t;


node_t *iterate(node_t *head, bool (*fun)(node_t*)) {
  node_t *cur = head;
  while(cur) {
    if(fun(cur)) {
      return cur;
    }
    cur = cur->next;
  }
  return cur;
}

// closure: remove

struct {
  int lock;
  node_t *head;
} heaps[MAXCPU];



static void *kalloc(size_t size) {
  return NULL;
}

static void kfree(void *ptr) {
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  uintptr_t pusize = pmsize / (MAXCPU > cpu_count() ? cpu_count() : MAXCPU);
  if(cpu_current() == 0) {
    for(int i = 0; i < (MAXCPU < cpu_count() ? MAXCPU : cpu_count()); i++) {
      heaps[i].head = heap.start + i * pusize;
      heaps[i].lock = 0;
      heaps[i].head->size = pusize - sizeof(node_t);
      heaps[i].head->prev = NULL;
      heaps[i].head->next = NULL;
    }
    for(int i = 0; i < cpu_count(); i++) {
      printf("cpu %d, heap size : %d\n", cpu_current(), heaps[i % MAXCPU].head->size);
    }
  }
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
