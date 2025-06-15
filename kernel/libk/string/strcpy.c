#include <libk/string.h>

void string_strcpy(string dest, const string src) {
    int i = 0;
    while (i < strlen(src)) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}