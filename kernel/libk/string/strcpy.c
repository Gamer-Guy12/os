#include <libk/string.h>

void string_strcpy(char *dest, const char *src) {
    int i = -1;
    while (src[i++]) {
        dest[i]=src[i];
    }
}