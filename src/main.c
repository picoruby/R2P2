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
#include "../build/mrb/usb_task.c"

#ifndef HEAP_SIZE
#define HEAP_SIZE (1024 * 200 - 1)
#endif

int loglevel = LOGLEVEL_ERROR;
static uint8_t heap_pool[HEAP_SIZE];

void
c_cdc_task(mrbc_vm *vm, mrbc_value v[], int argc)
{
  // connected() check for DTR bit
  // Most but not all terminal client set this when making connection
  // if ( tud_cdc_connected() )
  {
    // connected and there are data available
    if ( tud_cdc_available() )
    {
      // read datas
      char buf[64];
      uint32_t count = tud_cdc_read(buf, sizeof(buf));
      (void) count;

      // Echo back
      // Note: Skip echo by commenting out write() and write_flush()
      // for throughput test e.g
      //    $ dd if=/dev/zero of=/dev/ttyACM0 count=10000
      tud_cdc_write(buf, count);
      tud_cdc_write_flush();
    }
  }
}

void
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
  mrbc_define_method(0, mrbc_class_object, "tud_task", c_tud_task);
  mrbc_define_method(0, mrbc_class_object, "cdc_task", c_cdc_task);
  mrbc_require_init();
  mrbc_create_task(usb_task, 0);
  mrbc_create_task(app, 0);
  mrbc_run();
  return 0;
}

