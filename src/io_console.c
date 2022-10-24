#include <stdlib.h>

#include <mrubyc.h>

#define BUF_SIZE 64
#define STDOUT_FD 0

static void
c_gets_nonblock(mrb_vm *vm, mrb_value *v, int argc)
{
  int len = 0;
  int c;
  char buf[BUF_SIZE];
  while (hal_read(STDOUT_FD, &c, 1)) {
    buf[len] = c;
    len++;
    if (c == 10) break;
  }
  buf[len] = 0;
  mrbc_value value = mrbc_string_new(vm, buf, len);
  SET_RETURN(value);
}

static void
c_getch(mrb_vm *vm, mrb_value *v, int argc)
{
  int c;
  int res = hal_read(STDOUT_FD, &c, 1);
  if (res != 1) {
    SET_NIL_RETURN();
  } else {
    SET_INT_RETURN(c);
  }
}

static void
c_get_cursor_position(mrb_vm *vm, mrb_value *v, int argc)
{
  char buf[10];
  char *p1 = buf;
  char *p2 = NULL;
  char c;

  int row = 0;
  int col = 0;
  int res;

  // report cursor position
  res = hal_write(1, "\e[6n", 4);
  if (res != 4) mrbc_raise(vm, MRBC_CLASS(RuntimeError), "write() failed");

  for (;;) {
    res = hal_read(0, &c, 1);
    // if (res < 0) ignore
    if (res == 0) break;
    if(0x30 <= c && c <= 0x39) *p1++ = c;
    if(c == ';') {
      *p1++ = '\0';
      row = atoi(buf);
      p2 = p1;
    }
    if(c == 'R') break;
  }
  *p1 = '\0';
  col = atoi(p2);

  mrbc_value val_array = mrbc_array_new(vm, 2);
  val_array.array->n_stored = 2;
  if (row && col) {
    mrbc_set_integer(val_array.array->data, row);
    mrbc_set_integer(val_array.array->data + 1, col);
  } else {
    mrbc_raise(vm, MRBC_CLASS(Exception), "get_cursor_position failed");
  }
  SET_RETURN(val_array);
}

void
mrbc_io_console_init(void)
{
  mrbc_define_method(0, mrbc_class_object, "gets_nonblock", c_gets_nonblock);
  mrbc_define_method(0, mrbc_class_object, "getch", c_getch);
  mrbc_define_method(0, mrbc_class_object, "get_cursor_position", c_get_cursor_position);
}

