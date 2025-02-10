/* Raspi SDK */
#include <pico/stdlib.h>
#include <pico/bootrom.h>
#include <bsp/board.h>
#include <tusb.h>
#include <hardware/clocks.h>

/* PicoRuby */
#include <picogem_init.c>
#include <executables_init.c>

#include "main_task.c"

#if !defined(HEAP_SIZE)
  #if defined(PICO_RP2040)
    #if defined(USE_WIFI)
      #define HEAP_SIZE (1024 * 150)
    #else
      #define HEAP_SIZE (1024 * 194)
    #endif
  #elif defined(PICO_RP2350)
    #define HEAP_SIZE (1024 * (194 + 260))
  #else
    #error "Unknown board"
  #endif
#endif

static uint8_t heap_pool[HEAP_SIZE];

int
main(void)
{
  stdio_init_all();
  board_init();

  mrbc_init(heap_pool, HEAP_SIZE);
  mrbc_tcb *main_tcb = mrbc_create_task(main_task, 0);
  mrbc_set_task_name(main_tcb, "main_task");
  mrbc_vm *vm = &main_tcb->vm;
  picoruby_init_require(vm);
  picoruby_init_executables(vm);
  mrbc_run();
  return 0;
}

