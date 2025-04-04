/* Raspi SDK */
#include <pico/stdlib.h>
#include <pico/bootrom.h>
#include <bsp/board.h>
#include <tusb.h>
#include <hardware/clocks.h>

/* PicoRuby */
#include "picoruby.h"
#include "main_task.c"

#if !defined(HEAP_SIZE)
  #if defined(PICO_RP2040)
    #if defined(USE_WIFI)
      #define HEAP_SIZE (1024 * 150)
    #else
      #define HEAP_SIZE (1024 * 194)
    #endif
  #elif defined(PICO_RP2350)
    #define HEAP_SIZE (1024 * (194 + 235))
  #else
    #error "Unknown board"
  #endif
#endif

#if defined(PICORB_ALLOC_DEFAULT)
#define heap_pool NULL;
#else
static uint8_t heap_pool[HEAP_SIZE];
#endif

mrb_state *global_mrb = NULL;

int
main(void)
{
  stdio_init_all();
  board_init();

  int ret = 0;
  mrb_state *mrb = mrb_open_with_custom_alloc(heap_pool, HEAP_SIZE);
  global_mrb = mrb;
  mrc_irep *irep = mrb_read_irep(mrb, main_task);
  mrc_ccontext *cc = mrc_ccontext_new(mrb);
  mrb_tcb *tcb = mrc_create_task(cc, irep, NULL, "R2P2");
  tcb->c.ci->stack[0] = mrb_obj_value(mrb->top_self);
  if (!tcb) {
    fprintf(stderr, "mrbc_create_task failed\n");
    ret = 1;
  }
  else {
    mrb_tasks_run(mrb);
  }
  mrb_close(mrb);
  mrc_ccontext_free(cc);
  return ret;
}

