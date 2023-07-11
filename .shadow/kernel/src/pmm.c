#include <common.h>

#define MAGIC 0xff // warning: MAGIC must not alias to physical address

typedef struct __node_t {
    int              size;
    struct __node_t *next;
} node_t;

static node_t *head;



static node_t *iterate(node_t **head, bool (*fun)(node_t*)) {
  while(*head) {
    if(fun(*head)) {
      return *head;
    }
    *head = (*head)->next;
  }
  return NULL;
}

//**************************************
// closure 
static size_t msize;
static bool find_greater(node_t *cur) {
  if(cur->size >= msize) {
    return true;
  }
  return false;
}
//**************************************

static void *kalloc(size_t size) {
  node_t **cur = &head;
  msize = size;
  if(iterate(cur, find_greater) != NULL) {
    void* ret = (void*) (*cur) + sizeof(node_t);
    node_t *nxt = (*cur)->next;
    (*cur) = nxt;
    return ret;
  }
  return NULL;
}

//**************************************
// closure
static node_t* fnode;
static bool find_address(node_t *cur) {
  if(cur->next > fnode) {
    return true;
  }
  return false;
}
//**************************************

static void kfree(void *ptr) {
  node_t *nptr = ptr - sizeof(node_t);
  node_t **cur = &head;
  assert(nptr->next != (node_t*)MAGIC);
  fnode = nptr;
  iterate(cur, find_address);
  node_t *nxt = (*cur)->next;
  (*cur) = nptr;
  nptr->next = nxt;
  //then merging ipt, cur, cur->next if possible
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
