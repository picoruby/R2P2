/* Raspi SDK */
#include <pico/stdlib.h>
#include <pico/bootrom.h>
#include <bsp/board.h>
#include <tusb.h>
#include <hardware/clocks.h>

/* PicoRuby */
#include <picogem_init.c>
#include <executables_init.c>

#include "r2p2_task.c"
#include "usb_task.c"

#ifndef HEAP_SIZE

#ifdef USE_WIFI
#define HEAP_SIZE (1024 * 110)
//                (1025 * 157) is max size that can be built but it doesn't work well
// It'd be nice if we can use custom allocator in LwIP
#else
#define HEAP_SIZE (1024 * 194)
#endif

#endif

static uint8_t heap_pool[HEAP_SIZE];

static void
c_tud_task(mrbc_vm *vm, mrbc_value v[], int argc)
{
  tud_task();
}

int
main(void)
{
  stdio_init_all();
  board_init();
  tusb_init();

  mrbc_init(heap_pool, HEAP_SIZE);
  mrbc_tcb *tcb = mrbc_create_task(usb_task, 0);
  mrbc_vm *vm = &tcb->vm;
  mrbc_class *mrbc_class_USB = mrbc_define_class(vm, "USB", mrbc_class_object);
  mrbc_define_method(vm, mrbc_class_USB, "tud_task", c_tud_task);
  picoruby_init_require(vm);
  picoruby_init_executables(vm);
  mrbc_create_task(r2p2_task, 0);
  mrbc_run();
  return 0;
}

