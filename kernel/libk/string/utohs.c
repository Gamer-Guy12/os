#include <libk/string.h>

void string_utohs8(uint8_t num, char* buf) {
    uint8_t resNum = 1;
    uint16_t index = 0;

    if (num == 0) {
        buf[0] = '0';
        buf[1] = 'x';
        buf[2] = '0';
        buf[3] = '\0';
        return;
    }

    while (num > 0) {
        resNum = num % 16;
        num = (num - resNum) / 16;

        if (resNum < 10)
            buf[index] = resNum + '0';
        else
            buf[index] = resNum + 'A' - 10;

        index++;
    }

    // Creates random zero at the front so its a hack to get rid of it and also add the null termination
    // Also i have to add the like beginning 0x before the string gets reversed
    buf[index] = 'x';
    buf[index + 1] = '0';
    buf[index + 2] = '\0';

    strrev(buf);
}

void string_utohs16(uint16_t num, char* buf) {
    uint16_t resNum = 1;
    uint16_t index = 0;

    if (num == 0) {
        buf[0] = '0';
        buf[1] = 'x';
        buf[2] = '0';
        buf[3] = '\0';
        return;
    }

    while (num > 0) {
        resNum = num % 16;
        num = (num - resNum) / 16;

        if (resNum < 10)
            buf[index] = resNum + '0';
        else
            buf[index] = resNum + 'A' - 10;

        index++;
    }

    // Creates random zero at the front so its a hack to get rid of it and also add the null termination
    // Also i have to add the like beginning 0x before the string gets reversed
    buf[index] = 'x';
    buf[index + 1] = '0';
    buf[index + 2] = '\0';

    strrev(buf);
}

void string_utohs32(uint32_t num, char* buf) {
    uint32_t resNum = 1;
    uint16_t index = 0;

    if (num == 0) {
        buf[0] = '0';
        buf[1] = 'x';
        buf[2] = '0';
        buf[3] = '\0';
        return;
    }

    while (num > 0) {
        resNum = num % 16;
        num = (num - resNum) / 16;

        if (resNum < 10)
            buf[index] = resNum + '0';
        else
            buf[index] = resNum + 'A' - 10;

        index++;
    }

    // Creates random zero at the front so its a hack to get rid of it and also add the null termination
    // Also i have to add the like beginning 0x before the string gets reversed
    buf[index] = 'x';
    buf[index + 1] = '0';
    buf[index + 2] = '\0';

    strrev(buf);
}

void string_utohs64(uint64_t num, char* buf) {
    uint64_t resNum = 1;
    uint16_t index = 0;

    if (num == 0) {
        buf[0] = '0';
        buf[1] = 'x';
        buf[2] = '0';
        buf[3] = '\0';
        return;
    }

    while (num > 0) {
        resNum = num % 16;
        num = (num - resNum) / 16;

        if (resNum < 10)
            buf[index] = resNum + '0';
        else
            buf[index] = resNum + 'A' - 10;

        index++;
    }

    // Creates random zero at the front so its a hack to get rid of it and also add the null termination
    // Also i have to add the like beginning 0x before the string gets reversed
    buf[index] = 'x';
    buf[index + 1] = '0';
    buf[index + 2] = '\0';

    strrev(buf);
}
