#include <libk/string.h>

void strcpy(char *dest, const char *src) {
    int i = -1;
    while (src[i++]) {
        dest[i]=src[i];
    }
    dest[i]= '\0'
}