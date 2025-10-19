#include "lib/io.h"
#include "lib/string.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

static void add_char(char *buf, size_t buf_size, size_t *cur_size, char c) {
  if (buf_size > *cur_size + 1) {
    buf[*cur_size] = c;
  } else if (buf_size == *cur_size + 1) {
    buf[*cur_size] = '\0';
  }

  *cur_size += 1;
}

void print_hex(char *buf, size_t n, size_t *size, uint64_t num) {
  bool cur_leading_zeros = true;

  for (int i = 15; i >= 0; i--) {
    // Mask only the bits we want (0xF masks 4 bits)
    uint8_t value = (num >> (i * 4)) & 0xF;

    if (value == 0 && cur_leading_zeros) {
      continue;
    }

    cur_leading_zeros = false;

    char c = 0;

    if (value < 10) {
      c = '0' + value;
    } else {
      c = 'A' + value - 10;
    }

    add_char(buf, n, size, c);
  }
}

// Not preferred, would recommned printing hex when possible
void print_decimal(char *buf, size_t n, size_t *size, uint64_t num) {
  uint64_t resnum = 0;
  uint16_t index = 0;
  char str[20];

  if (num == 0) {
    str[0] = '0';
    str[1] = '\0';
  } else {
    while (num > 0) {
      resnum = num % 10;
      num = (num - resnum) / 10;

      str[index] = resnum + '0';
      index++;
    }
    str[index] = '\0';
    strrev(str);
  }

  index = 0;
  while (index < 20 && str[index] != '\0') {
    add_char(buf, n, size, str[index]);
  }
}

void print_int(char *buf, size_t n, size_t *size, int64_t num) {
  uint64_t val = num;

  if (num < 0) {
    add_char(buf, n, size, '-');
    // Make positive
    val = ~num + 1;
  }

  print_decimal(buf, n, size, val);
}

// Supported format specifiers: %x - unsigned hex (64 bit), %u - unsigned
// decimal (64 bit), %c - char, %d - signed decimal (64 bit)
int vsnprintf(char *buf, size_t n, const char *format, va_list args) {
  size_t size = 0;
  size_t char_index = 0;

  while (format[char_index] != '\0') {
    if (format[char_index] != '%') {
      add_char(buf, n, &size, format[char_index]);
      char_index++;
      continue;
    }

    // Skip the format specifier %
    char_index++;

    switch (format[char_index]) {
    case 'x': {
      uint64_t x = va_arg(args, uint64_t);
      print_hex(buf, n, &size, x);
    } break;
    case 'u': {
      uint64_t u = va_arg(args, uint64_t);
      print_decimal(buf, n, &size, u);
    } break;
    case 'c': {
      char c = va_arg(args, int);
      add_char(buf, n, &size, c);
    } break;
    case 'd': {
      int64_t d = va_arg(args, int64_t);
      print_int(buf, n, &size, d);
    } break;
    default:
      return -1;
    }

    // Skip the format specifier
    char_index++;
  }

  return size;
}
