#include <libk/string.h>

/// This was copied from google gemini
void strrev(char* buf) {
    uint16_t length = strlen(buf);
    uint16_t i, j;
    char temp;

    for (i = 0, j = length - 1; i < j; i++, j--) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
    }
}
