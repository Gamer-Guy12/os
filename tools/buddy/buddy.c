#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

uint8_t* data;
#define BUDDY_MAX_ORDER 9

static void set_bit_in_ptr(uint8_t* ptr, size_t bit) {
    size_t index = bit - (bit % 8);
    size_t offset = bit - index;
    index /= 8;

    ptr[index] |= 1 << offset;
}

static void reserve_page(size_t page) {
    // Reserve descriptor

    size_t previous_bit = 0;

    for (size_t i = BUDDY_MAX_ORDER; i > 0; i--) {
        if (i == BUDDY_MAX_ORDER) {
            data[0] |= 1;
            continue;
        }

        size_t size = powl(2, i + 1);
        bool is_lower = page % size < size / 2;

        if (is_lower) {
            previous_bit = previous_bit * 2 + 1;
            set_bit_in_ptr(data, previous_bit);
        } else {
            previous_bit = previous_bit * 2 + 2;
            set_bit_in_ptr(data, previous_bit);
        }
    }

    if (page % 2 == 0) {
        previous_bit = previous_bit * 2 + 1;
        set_bit_in_ptr(data, previous_bit);
    } else {
        previous_bit = previous_bit * 2 + 2;
        set_bit_in_ptr(data, previous_bit);
    }
}

static void print_pages() {
    for (size_t i = 0; i < 128; i += 8) {
        printf("%x %x %x %x %x %x %x %x\n", data[i + 0], data[i + 1], data[i + 2], data[i + 3], data[i + 4], data[i + 5], data[i + 6], data[i + 7]);
    }
}

static void print_indices(bool is_one) {
    for (size_t i = 0; i < 128; i++) {
        for (size_t j = 0; j < 8; j++) {
            if (is_one && (data[i] & (1 << j))) {
                printf("index: %zu\n", i * 8 + j);
            }
        }
    }
}

int main(int argv, char** argc) {
    data = malloc(sizeof(uint8_t) * 128);

    for (size_t i = 0; i < 0x5D; i++) {
        reserve_page(i);
    }

    print_pages();

    print_indices(false);
    
    free(data);

    return 0;
}