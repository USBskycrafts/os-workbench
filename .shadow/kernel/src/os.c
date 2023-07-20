#include <common.h>

static void os_init() {
  pmm->init();
}

void* address[1000];

static void os_run() {
  for (const char *s = "Hello World from CPU #*\n"; *s; s++) {
    putch(*s == '*' ? '0' + cpu_current() : *s);
  }
  memset(address, 0, sizeof(address));
  pmm->free(pmm->alloc(1));
  // while(1) {
  //   size_t index = rand() % 1000;
  //   if(address[index] == NULL) {
  //     address[index] = pmm->alloc(rand() % (1 << 24));
  //   } else {
  //     pmm->free(address[index]);
  //     address[index] = NULL;
  //   }
  // }
  printf("finished\n");
  while(1);
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
