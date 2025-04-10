#include <libk/string.h>

void string_utos8(uint8_t num, char* buf) {
    uint8_t resNum = 1;
    uint16_t index = 0;

    while (resNum != 0) {
        resNum = num % 10;
        num = (num - resNum) / 10;

        buf[index] = resNum + '0';

        index++;
    }

    // Creates random zero at the front so its a hack to get rid of it and also add the null termination
    buf[index - 1] = '\0';

    strrev(buf);
}

void string_utos16(uint16_t num, char* buf) {
    uint16_t resNum = 1;
    uint16_t index = 0;

    while (resNum != 0) {
        resNum = num % 10;
        num = (num - resNum) / 10;

        buf[index] = resNum + '0';

        index++;
    }

    // Creates random zero at the front so its a hack to get rid of it and also add the null termination
    buf[index - 1] = '\0';

    strrev(buf);
}

void string_utos32(uint32_t num, char* buf) {
    uint32_t resNum = 1;
    uint16_t index = 0;

    while (resNum != 0) {
        resNum = num % 10;
        num = (num - resNum) / 10;

        buf[index] = resNum + '0';

        index++;
    }

    // Creates random zero at the front so its a hack to get rid of it and also add the null termination
    buf[index - 1] = '\0';

    strrev(buf);
}

void string_utos64(uint64_t num, char* buf) {
    uint64_t resNum = 1;
    uint16_t index = 0;

    while (resNum != 0) {
        resNum = num % 10;
        num = (num - resNum) / 10;

        buf[index] = resNum + '0';

        index++;
    }

    // Creates random zero at the front so its a hack to get rid of it and also add the null termination
    buf[index - 1] = '\0';

    strrev(buf);
}
