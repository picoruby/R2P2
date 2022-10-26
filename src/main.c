/* Raspi SDK */
#include <pico/stdlib.h>
#include <pico/bootrom.h>
#include <bsp/board.h>
#include <tusb.h>
#include <hardware/clocks.h>

/* PicoRuby */
#include <picorbc.h>
#include <picogem_init.c>

#include "../build/mrb/app.c"

#ifndef HEAP_SIZE
#define HEAP_SIZE (1024 * 200 - 1)
#endif

int loglevel = 0;
static uint8_t heap_pool[HEAP_SIZE];

int
main(void)
{
  stdio_init_all();
  board_init();
  tusb_init();

  mrbc_init(heap_pool, HEAP_SIZE);
  mrbc_require_init();
  mrbc_create_task(app, 0);
  mrbc_run();
  return 0;
}

