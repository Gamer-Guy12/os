#ifndef _LIB_STRING_H_
#define _LIB_STRING_H_

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *ptr, int v, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

size_t strlen(const char *s);
char *strcpy(char *dest, const char* src);
char *strrev(char *str);

#endif
