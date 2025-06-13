#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PHYS_BUDDY_MAX_ORDER 9

static void set_bit_in_ptr(uint8_t *ptr, size_t bit) {
  size_t index = bit - (bit % 8);
  size_t offset = bit - index;
  index /= 8;

  ptr[index] |= 1 << offset;
}

static bool check_bit_in_ptr(uint8_t *ptr, size_t bit) {
  size_t index = bit - (bit % 8);
  size_t offset = bit - index;
  index /= 8;

  return ptr[index] & (1 << offset);
}

static void reserve_page(uint8_t *data, size_t page) {
  // Reserve descriptor

  size_t previous_bit = 0;

  for (size_t i = PHYS_BUDDY_MAX_ORDER; i > 0; i--) {
    if (i == PHYS_BUDDY_MAX_ORDER) {
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

static void print_pages(uint8_t *data) {
  for (size_t i = 0; i < 128; i += 8) {
    printf("%x %x %x %x %x %x %x %x\n", data[i + 0], data[i + 1], data[i + 2],
           data[i + 3], data[i + 4], data[i + 5], data[i + 6], data[i + 7]);
  }
}

static void print_indices(uint8_t *data, bool is_one) {
  for (size_t i = 0; i < 128; i++) {
    for (size_t j = 0; j < 8; j++) {
      if (is_one && (data[i] & (1 << j))) {
        printf("index: %zu\n", i * 8 + j);
      }
    }
  }
}

static bool check_for_page(uint8_t *data, size_t page) {
  size_t previous_bit = 0;

  for (size_t i = PHYS_BUDDY_MAX_ORDER; i > 0; i--) {
    if (i == PHYS_BUDDY_MAX_ORDER) {
      if (!(data[0] & 1)) {
        return false;
      }
      continue;
    }

    size_t size = powl(2, i + 1);
    bool is_lower = page % size < size / 2;

    if (is_lower) {
      previous_bit = previous_bit * 2 + 1;
    } else {
      previous_bit = previous_bit * 2 + 2;
    }

    if (!check_bit_in_ptr(data, previous_bit)) {
      return false;
    }
  }

  return true;
}

static void do_buddy_test(size_t count) {
  uint8_t *data = malloc(sizeof(uint8_t) * 128);

  for (size_t i = 0; i < count; i++) {
    reserve_page(data, i);
  }

  for (size_t i = 0; i < count; i++) {
    if (!check_for_page(data, i)) {
      printf("Failed index %zu!\n", i);
    }
  }

  free(data);
}

int main(int argv, char **argc) {
  for (size_t i = 0; i <= 512; i++) {
    do_buddy_test(i);
  }

  return 0;
}