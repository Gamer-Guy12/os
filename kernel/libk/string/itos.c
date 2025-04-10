#include <libk/string.h>
#include <stdbool.h>

void string_itos8(int8_t num, char* buf) {
    uint8_t resNum = 1;
    uint16_t index = 0;
    bool negate = num < 0;

    if (negate) num *= -1;

    while (resNum != 0) {
        resNum = num % 10;
        num = (num - resNum) / 10;

        buf[index] = resNum + '0';

        index++;
    }

    if (negate) {
        buf[index - 1] = '-';
        buf[index] = '\0';
    } else {
        buf[index - 1] = '\0';
    }

    strrev(buf);
}

void string_itos16(int16_t num, char* buf) {
    uint8_t resNum = 1;
    uint16_t index = 0;
    bool negate = num < 0;

    if (negate) num *= -1;

    while (resNum != 0) {
        resNum = num % 10;
        num = (num - resNum) / 10;

        buf[index] = resNum + '0';

        index++;
    }

    if (negate) {
        buf[index - 1] = '-';
        buf[index] = '\0';
    } else {
        buf[index - 1] = '\0';
    }

    strrev(buf);
}

void string_itos32(int32_t num, char* buf) {
    uint8_t resNum = 1;
    uint16_t index = 0;
    bool negate = num < 0;

    if (negate) num *= -1;

    while (resNum != 0) {
        resNum = num % 10;
        num = (num - resNum) / 10;

        buf[index] = resNum + '0';

        index++;
    }

    if (negate) {
        buf[index - 1] = '-';
        buf[index] = '\0';
    } else {
        buf[index - 1] = '\0';
    }

    strrev(buf);
}

void string_itos64(int64_t num, char* buf) {
    uint8_t resNum = 1;
    uint16_t index = 0;
    bool negate = num < 0;

    if (negate) num *= -1;

    while (resNum != 0) {
        resNum = num % 10;
        num = (num - resNum) / 10;

        buf[index] = resNum + '0';

        index++;
    }

    if (negate) {
        buf[index - 1] = '-';
        buf[index] = '\0';
    } else {
        buf[index - 1] = '\0';
    }

    strrev(buf);
}
