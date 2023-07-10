#include <common.h>


#define MAGIC 0xff

typedef struct __node_t {
    int              size;
    struct __node_t *next;
} node_t;

static node_t *head;



static node_t* iterate(node_t *head, bool (*fun)(node_t*)) {
  node_t *ptr = head;
  while(ptr) {
    if(fun(ptr)) {
      return ptr;
    }
    ptr = ptr->next;
  }
  return NULL;
}

//**************************************
// closure 
static size_t msize;
static bool find_greater(node_t* cur) {
  if(cur->size >= msize) {
    return true;
  }
  return false;
}
//**************************************

static void *kalloc(size_t size) {
  node_t *cur;
  msize = size;
  if((cur = iterate(head, find_greater)) != NULL) {
    void* ret = (void*) cur + sizeof(node_t);
    head = cur->next;
    cur->next = (node_t*)0xff;
    return ret;
  }
  return NULL;
}

static void kfree(void *ptr) {

}

static void pmm_init() {
  uintptr_t pmsize = ((uintptr_t)heap.end - (uintptr_t)heap.start);
  printf("Got %d MiB heap: [%p, %p)\n", pmsize >> 20, heap.start, heap.end);
  head = (node_t*)heap.start;
  head->next = NULL;
  head->size = pmsize;
}

MODULE_DEF(pmm) = {
  .init  = pmm_init,
  .alloc = kalloc,
  .free  = kfree,
};
