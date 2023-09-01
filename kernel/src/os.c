#include <common.h>

static void os_init() {
  pmm->init();
}

static void os_run() {
  
}

MODULE_DEF(os) = {
  .init = os_init,
  .run  = os_run,
};
