#include <libk/string.h>

void string_strcpy(char *dest, const char *src) {
    char *buffer = dest;
    int i = 0;
    while (src[i++]) {
        buffer[i]=src[i]
    }
    return buffer;
}