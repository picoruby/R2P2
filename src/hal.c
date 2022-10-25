#include <string.h>
#include <stdlib.h>

/* pico-sdk and tinyusb */
#include <pico/stdlib.h>
#include <hardware/irq.h>
#include <hardware/clocks.h>
#include <hardware/sync.h>
//#include "../include/tusb_config.h"
#include <tusb.h>

#include "../lib/picoruby/mrbgems/picoruby-io/src/hal/hal.h"

/* mruby/c */
#include <rrt0.h>

#define ALARM_IRQ 0

void hal_enable_irq()  { irq_set_enabled(ALARM_IRQ, true); }
void hal_disable_irq() { irq_set_enabled(ALARM_IRQ, false); }
void hal_idle_cpu()    { __wfi(); }

struct repeating_timer timer;

//================================================================
/*!@brief
  timer alarm irq

*/
bool alarm_irq(struct repeating_timer *t) {
  mrbc_tick();
  return false;
}

//================================================================
/*!@brief
  initialize

*/
void hal_init(void){
  add_repeating_timer_ms(1, alarm_irq, NULL, &timer);
  clocks_hw->sleep_en0 = 0;
  clocks_hw->sleep_en1 = CLOCKS_SLEEP_EN1_CLK_SYS_TIMER_BITS
                        | CLOCKS_SLEEP_EN1_CLK_SYS_USBCTRL_BITS
                        | CLOCKS_SLEEP_EN1_CLK_USB_USBCTRL_BITS
                        | CLOCKS_SLEEP_EN1_CLK_SYS_UART0_BITS
                        | CLOCKS_SLEEP_EN1_CLK_PERI_UART0_BITS;
}

int hal_write(int fd, const void *buf, int nbytes)
{
  tud_cdc_write(buf, nbytes);
  return tud_cdc_write_flush();
}

int hal_flush(int fd) {
  return tud_cdc_write_flush();
}

int
hal_read_available(int fd)
{
  if (tud_cdc_available()) {
    return 1;
  } else {
    return 0;
  }
}

int
hal_read_nonblock(int fd, char *buf, int nbytes)
{
  int len = tud_cdc_read(buf, nbytes);
  return len;
}

int
hal_read_char(int fd)
{
  while (!hal_read_available(STDIN_FD)) {
    sleep_ms(5);
  }
  int c = tud_cdc_read_char();
  if (c < 0) {
    return -1;
  } else {
    return c;
  }
}

//================================================================
/*!@brief
  abort program

  @param s	additional message.
*/
void hal_abort(const char *s)
{
  if( s ) {
    hal_write(1, s, strlen(s));
  }

  abort();
}

