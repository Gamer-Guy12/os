#ifndef _LIB_IO_H_
#define _LIB_IO_H_

#include <stdarg.h>
#include <stddef.h>

int vsnprintf(char *buf, size_t n, const char *format, va_list args);

#endif
