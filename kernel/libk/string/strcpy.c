#include <libk/string.h>

void strcpy(char *dest, const char *src) {
    int i = 0;
    while (src[i]) {
        dest[i]=src[i];
        i++;
    }
    dest[i]= '\0'
}