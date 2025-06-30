#include <libk/kgfx.h>
#include <libk/kio.h>
#include <libk/spinlock.h>
#include <libk/string.h>
#include <stdarg.h>
#include <stdint.h>

static spinlock_t printLock;

/// This is a cut down version of printf
/// The format specifiers that you can use are:
/// %d (assumed to be 64 bit), %u (assumed to be 64 bit), %x, %f (not yet)
/// (assumes double), %c, %s, %%
void kio_printf(const char *format, ...) {
  spinlock_acquire(&printLock);

  uint16_t index = 0;
  va_list args;
  va_start(args, format);
  static char str[24];

  while (format[index] != '\0') {
    if (format[index] != '%') {
      kgfx_putchar(format[index]);
    } else {

      index++;

      switch (format[index]) {
      case 'd':;
        int64_t vald = va_arg(args, int64_t);
        string_itos64(vald, str);
        break;
      case 'u':;
        uint64_t valu = va_arg(args, uint64_t);
        string_utos64(valu, str);
        break;
      // case 'f':;
      //     double valf = va_arg(args, double);
      //     string_ftos64(valf, str);
      //     break;
      case 'x':;
        uint64_t valx = va_arg(args, uint64_t);
        string_utohs64(valx, str);
        break;
      case 'c':;
        str[0] = 0xFF & va_arg(args, uint32_t);
        str[1] = '\0';
        break;
      case 's':;
        kio_puts(va_arg(args, char *));
        str[0] = '\0';
        break;
      case '%':;
        str[0] = '%';
        str[1] = '\0';
      }

      kio_puts(str);
    }
    index++;
  }

  va_end(args);
  spinlock_release(&printLock);
}
