/* Raspi SDK */
#include <pico/stdlib.h>
#include <pico/bootrom.h>
#include <bsp/board.h>
#include <tusb.h>
#include <hardware/clocks.h>

#include <stdio.h>

/* PicoRuby */
#include "picoruby.h"
#include "main_task.c"

#if !defined(HEAP_SIZE)
  #if defined(PICO_RP2040)
    #if defined(USE_WIFI)
      #define HEAP_SIZE (1024 * 140)
    #else
      #define HEAP_SIZE (1024 * 194)
    #endif
  #elif defined(PICO_RP2350)
    #if defined(USE_WIFI)
      #define HEAP_SIZE (1024 * (150 + 200))
    #else
      #define HEAP_SIZE (1024 * (194 + 200))
    #endif
  #else
    #error "Unknown board"
  #endif
#endif

#if defined(R2P2_ALLOC_LIBC)
#define heap_pool NULL
#else
static uint8_t heap_pool[HEAP_SIZE] __attribute__((aligned(8)));
#endif

mrb_state *global_mrb = NULL;

int
main(void)
{
  stdio_init_all();
  printf("R2P2 PicoRuby starting...\n");
  printf("HEAP_SIZE: %d bytes\n", HEAP_SIZE);
  board_init();

  int ret = 0;

#if defined(PICORB_VM_MRUBY)
  mrb_state *mrb = mrb_open_with_custom_alloc(heap_pool, HEAP_SIZE);
  global_mrb = mrb;
  mrc_irep *irep = mrb_read_irep(mrb, main_task);
  mrc_ccontext *cc = mrc_ccontext_new(mrb);
  mrb_value name = mrb_str_new_lit(mrb, "R2P2");
  mrb_value task = mrc_create_task(cc, irep, name, mrb_nil_value(), mrb_obj_value(mrb->top_self));
  if (mrb_nil_p(task)) {
    printf("mrbc_create_task failed\n");
    ret = 1;
  }
  else {
    mrb_tasks_run(mrb);
  }
  if (mrb->exc) {
    mrb_print_error(mrb);
    ret = 1;
  }
  mrb_close(mrb);
  mrc_ccontext_free(cc);
#elif defined(PICORB_VM_MRUBYC)
  mrbc_init(heap_pool, HEAP_SIZE);
  mrbc_tcb *main_tcb = mrbc_create_task(main_task, 0);
  if (!main_tcb) {
    printf("mrbc_create_task failed\n");
    ret = 1;
  }
  else {
    mrbc_set_task_name(main_tcb, "main_task");
    mrbc_vm *vm = &main_tcb->vm;
    picoruby_init_require(vm);
    mrbc_run();
  }
#endif
  return ret;
}

