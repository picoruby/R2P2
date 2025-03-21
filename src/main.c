/* Raspi SDK */
#include <pico/stdlib.h>
#include <pico/bootrom.h>
#include <bsp/board.h>
#include <tusb.h>
#include <hardware/clocks.h>

/* PicoRuby */
#include "picoruby.h"
#include "main_task.c"

#ifndef HEAP_SIZE
#define HEAP_SIZE (1024 * 340)
#endif

static uint8_t heap_pool[HEAP_SIZE];

mrb_state *global_mrb = NULL;

int
main(void)
{
  stdio_init_all();
  board_init();

  int ret = 0;
  mrb_state *mrb = mrb_open_with_tlsf(heap_pool, HEAP_SIZE);
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

