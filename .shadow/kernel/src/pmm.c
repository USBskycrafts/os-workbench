#include <common.h>

#define MAGIC 0xff // warning: cannot overlap with physical address
#define MAX_CPU 8


#define cpu_id cpu_current() % MAX_CPU

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
    int              size;
    int              lock;
    struct __node_t *next;
    struct __node_t *prev;
} node_t;

static node_t *head[MAX_CPU];


static node_t *iterate(node_t **head, bool (*fun)(node_t*)) {
  while(*head) {
    if(fun(*head)) {
      return *head;
    }
    *head = (*head)->next;
  }
  return NULL;
}


static void remove(node_t **cur) {
  node_t *prev = (*cur)->prev;
  node_t *next = (*cur)->next;
  prev->next = next;
  if(next != NULL) {
    next->prev = prev;
  }
}

// closure
static node_t* addr;
static bool find_addr(node_t *cur) {
  if(addr < cur) {
    return true;
  }
  return false;
}
// ----------------------------

static void insert(node_t *n) {

  node_t **cur = &head[cpu_id];
  while(atomic_xchg(&head[cpu_id]->lock, 1));
  addr = n;
  iterate(cur, find_addr);

  node_t **prev = &((*cur)->prev);
  n->prev = *prev;
  (*prev) = n;
  n->next = (*cur);
  head[cpu_id]->lock = 0;
}

// closure
static size_t msize;
static bool find_greater(node_t *cur) {
  if(cur->size >= msize) {
    return true;
  }
  return false;
}

static node_t *split(node_t *cur) {
  if(cur->size - msize > 2 + sizeof(node_t)) {
    // able to split, return the new node's header
    node_t *ret = (void*) cur + msize + sizeof(node_t);
    ret->size = cur->size - msize - sizeof(node_t);
  }
  // cannot split, return NULL
  return NULL;
}

// ---------------------------



static void *kalloc(size_t size) {
  if(size == 0) {
    return NULL;
  }
  size = alias(size);

  msize = size;
  node_t **ptr = &head[cpu_id];
  if(iterate(ptr, find_greater) != NULL) {
    remove(ptr);
    (*ptr)->size = size;
    node_t *split_n = split(*ptr);
    if(split_n != NULL) {
      insert(split_n);
    } 
    return (void*)ptr + sizeof(node_t);
  }
  return NULL;
}

static void kfree(void *ptr) {
}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  uintptr_t pusize = pmsize / cpu_count();
  if(cpu_current() == 0) {
    for(int i = 0; i < (cpu_count() > MAX_CPU ? MAX_CPU : cpu_count()); i++) {
      head[i] = (node_t*) heap.start + i * pusize;
      head[i]->size = pusize - sizeof(node_t);
      head[i]->lock = 0;
      head[i]->prev = NULL;
      head[i]->next = NULL;
    }
  }
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
