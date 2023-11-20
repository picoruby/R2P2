/* Raspi SDK */
#include <pico/stdlib.h>
#include <pico/bootrom.h>
#include <bsp/board.h>
#include <tusb.h>
#include <hardware/clocks.h>

/* PicoRuby */
#include <picorbc.h>
#include <picogem_init.c>
#include <executables_init.c>

#include "../build/mrb/r2p2_task.c"
#include "../build/mrb/usb_task.c"

#ifndef HEAP_SIZE
#define HEAP_SIZE (1024 * 194)
#endif

int loglevel = LOGLEVEL_ERROR;
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
  mrbc_class *mrbc_class_USB = mrbc_define_class(0, "USB", mrbc_class_object);
  mrbc_define_method(0, mrbc_class_USB, "tud_task", c_tud_task);
  picoruby_init_require();
  picoruby_init_executables();
  mrbc_create_task(usb_task, 0);
  mrbc_create_task(r2p2_task, 0);
  mrbc_run();
  return 0;
}

