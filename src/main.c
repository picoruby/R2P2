/* Raspi SDK */
#include <pico/stdlib.h>
#include <pico/bootrom.h>
#include <bsp/board.h>
#include <tusb.h>
#include <hardware/clocks.h>

/* PicoRuby */
#include "picoruby.h"
#include "main_task.c"

#if defined(PICORUBY_DEBUG)
#include "pico/stdio.h"
#include "hardware/uart.h"
#define DBG_UART       uart0
#define DBG_BAUD       115200
#define DBG_TX_PIN     0              // GPIO0 ＝ UART0 TX
#define DBG_RX_PIN     1              // GPIO1 ＝ UART0 RX
#define DBG_BUF_SIZE   128

static void
dbg_uart_init(void)
{
  uart_init(DBG_UART, DBG_BAUD);
  gpio_set_function(DBG_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(DBG_RX_PIN, GPIO_FUNC_UART);
}

void
d(const char *fmt, ...)
{
  char buf[DBG_BUF_SIZE];
  va_list ap;
  va_start(ap, fmt);
  int n = vsnprintf(buf, sizeof(buf) - 2, fmt, ap);
  va_end(ap);
  if (n < 0) return;
  if (n > (sizeof(buf) - 3)) n = sizeof(buf) - 3;
  buf[n++] = '\r';
  buf[n++] = '\n';
  buf[n]   = '\0';
  uart_write_blocking(DBG_UART, (uint8_t *)buf, n);
}

#else

static void dbg_uart_init(void)
{
}

void d(const char *fmt, ...)
{
  (void)fmt; // No-op if debugging is not enabled
}
#endif

#if !defined(HEAP_SIZE)
  #if defined(PICO_RP2040)
    #if defined(USE_WIFI)
      #define HEAP_SIZE (1024 * 150)
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
  dbg_uart_init();
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
    fprintf(stderr, "mrbc_create_task failed\n");
    ret = 1;
  }
  else {
    mrb_tasks_run(mrb);
  }
  mrb_close(mrb);
  mrc_ccontext_free(cc);
#elif defined(PICORB_VM_MRUBYC)
  mrbc_init(heap_pool, HEAP_SIZE);
  mrbc_tcb *main_tcb = mrbc_create_task(main_task, 0);
  if (!main_tcb) {
    fprintf(stderr, "mrbc_create_task failed\n");
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

