#include <common.h>

#define MAGIC 0xff // warning: MAGIC must not alias to physical address

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
    //wrong, need bug fixing
    head = cur->next;
    cur->next = (node_t*)MAGIC;
    //----------------------
    return ret;
  }
  return NULL;
}

//**************************************
// closure
static node_t* fnode;
static bool find_address(node_t *cur) {
  if(cur->next > fnode && cur < fnode) {
    return true;
  }
  return false;
}
//**************************************

static void kfree(void *ptr) {
  node_t* cur = ptr - sizeof(node_t);
  if(cur->next !=  (node_t*) MAGIC) {
    char err[35] = "free after free at";
    char addr[12];
    sprintf(addr, " %p\n", ptr);
    strcat(err, addr);
    panic(err);
  }
  fnode = cur;
  node_t *ipt = iterate(head, find_address);
  cur->next = ipt->next;
  ipt->next = cur;
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
