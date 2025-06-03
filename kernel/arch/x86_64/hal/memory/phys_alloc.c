#include <hal/imemory.h>
#include <hal/memory.h>
#include <libk/err.h>
#include <libk/kio.h>
#include <libk/lock.h>
#include <libk/math.h>
#include <libk/sys.h>
#include <stddef.h>
#include <stdint.h>

inline static bool check_bit_in_ptr(const uint8_t *ptr, size_t bit) {
  size_t index = ROUND_DOWN(bit, 8);
  size_t offset = bit - index;

  // kio_printf("Value %x and ret %x and offset %x %x\n", (size_t)ptr[index],
  //            (size_t)ptr[index] & (1 << offset), offset, 1 << offset);

  return ptr[index / 8] & (1 << offset);
}

inline static void set_bit_in_ptr(uint8_t *ptr, size_t bit) {
  size_t index = ROUND_DOWN(bit, 8);
  size_t offset = bit - index;

  ptr[index / 8] |= 1 << offset;
}

static block_descriptor_t *get_descriptor(size_t base) {
  block_descriptor_t *blocks = (block_descriptor_t *)BLOCK_DESCRIPTORS_ADDR;

  for (size_t i = 0; i < get_block_count(); i++) {
    if (blocks[i].addr == (base >> 21)) {
      return &blocks[i];
    }
  }

  return NULL;
}

inline static bool block_of_order_exists(const uint8_t *ptr, size_t order) {
  size_t start_index = math_powu64(2, BUDDY_MAX_ORDER - order) - 1;
  size_t end_index = math_powu64(2, BUDDY_MAX_ORDER - order + 1) - 1;

  for (size_t i = 0; i < end_index - start_index; i++) {
    if (check_bit_in_ptr(ptr, start_index + i)) {
      return true;
    }
  }

  return false;
}

static block_descriptor_t *get_free_descriptor(void) {
  block_descriptor_t *descriptors =
      (block_descriptor_t *)BLOCK_DESCRIPTORS_ADDR;

  for (size_t i = 0; i < get_block_count(); i++) {
    // kio_printf("i: %x, free pages: %x, flags :%x\n", i,
    //            (size_t)descriptors[i].free_pages,
    //            (size_t)descriptors[i].flags);
    if (descriptors[i].free_pages > 0 &&
        !(descriptors[i].flags & BLOCK_DESCRIPTOR_RESERVED)) {
      return &descriptors[i];
    }
  }

  return NULL;
}

static void reserve_page(size_t block_base, size_t page) {
  block_descriptor_t *descriptor = get_descriptor(block_base);

  if (descriptor == NULL) {
    sys_panic(MEMORY_INIT_ERR | MISSING_BLOCK_ERR);
  }

  descriptor->free_pages--;

  size_t prev_bit = 0;

  for (size_t i = BUDDY_MAX_ORDER; i > 0; i--) {
    if (i == BUDDY_MAX_ORDER) {
      set_bit_in_ptr(descriptor->buddy_data, 0);
      continue;
    }

    size_t size = math_powu64(2, i);

    if (page % size < size / 2) {
      prev_bit = prev_bit * 2 + 1;
      set_bit_in_ptr(descriptor->buddy_data, prev_bit);
    } else {
      prev_bit = prev_bit * 2 + 2;
      set_bit_in_ptr(descriptor->buddy_data, prev_bit);
    }
  }

  if (page % 2 == 0) {
    prev_bit = prev_bit * 2 + 1;
    set_bit_in_ptr(descriptor->buddy_data, prev_bit);
  } else {
    prev_bit = prev_bit * 2 + 2;
    set_bit_in_ptr(descriptor->buddy_data, prev_bit);
  }

  for (size_t i = BUDDY_MAX_ORDER; i >= 0; i--) {
    if (block_of_order_exists(descriptor->buddy_data, i)) {
      descriptor->largest_region_order = i;
      return;
    }
  }

  descriptor->largest_region_order = 0;
}

static size_t find_pair(block_descriptor_t *descriptor, size_t order) {
  if (order == BUDDY_MAX_ORDER)
    return 0;

  size_t layer = BUDDY_MAX_ORDER - order;
  size_t start = math_powu64(2, layer) - 1;
  size_t length = math_powu64(2, layer + 1) - 1 - start;

  for (size_t i = start; i < start + length; i += 2) {
    if (check_bit_in_ptr(descriptor->buddy_data, i) &&
        !check_bit_in_ptr(descriptor->buddy_data, i + 1)) {
      return i + 1;
    } else if (!check_bit_in_ptr(descriptor->buddy_data, i) &&
               check_bit_in_ptr(descriptor->buddy_data, i + 1)) {
      return i;
    }
  }

  return 0;
}

static size_t find_free_page(block_descriptor_t *descriptor) {
  for (size_t i = 0; i < BUDDY_MAX_ORDER; i++) {
    size_t index = find_pair(descriptor, i);

    if (index != 0) {
      for (size_t j = i; j > 0; j--) {
        index = index * 2 + 1;
      }

      return index;
    }
  }

  return 0;
}

static bool check_page_index(block_descriptor_t *descriptor, size_t page) {
  uint8_t *data = descriptor->buddy_data;

  size_t prev_bit = 0;

  for (size_t i = BUDDY_MAX_ORDER; i > 0; i--) {
    if (i == BUDDY_MAX_ORDER) {
      if (!(data[0] & 1)) {
        // kio_printf("Index %x\n", i);
        return false;
      }
    } else {
      size_t size = math_powu64(2, i);

      if (page % size < size / 2) {
        prev_bit = prev_bit * 2 + 1;
        if (!(check_bit_in_ptr(data, prev_bit))) {
          // kio_printf("Index %x %x\n", i, prev_bit);
          return false;
        }
        // kio_printf("Lower %x  ", prev_bit);
      } else {
        prev_bit = prev_bit * 2 + 2;
        if (!(check_bit_in_ptr(data, prev_bit))) {
          // kio_printf("Idex %x %x\n", i, prev_bit);
          return false;
        }
        // kio_printf("Higher %x  ", prev_bit);
      }
    }
  }
  // kio_printf("\n");

  if (page % 2 == 0) {
    prev_bit = prev_bit * 2 + 1;
    if (!check_bit_in_ptr(data, prev_bit)) {
      // kio_printf("Inde %x\n", prev_bit);
      return false;
    }
  } else {
    prev_bit = prev_bit * 2 + 2;
    if (!(check_bit_in_ptr(data, prev_bit))) {
      // kio_printf("Ide %x\n", prev_bit);
      return false;
    }
  }

  return true;
}

void *phys_alloc(void) {
  lock_acquire(get_mem_lock());

  block_descriptor_t *descriptor = get_free_descriptor();

  if (descriptor == NULL)
    return NULL;

  size_t page = find_free_page(descriptor);

  kio_printf("Page %x\n", page);

  kio_printf("Exists %x\n", (size_t)check_page_index(descriptor, page));

  // uint8_t *data = descriptor->buddy_data;
  //
  // for (size_t i = 0; i < 128; i += 8) {
  //   kio_printf("Data: %x %x %x %x %x %x %x %x\n", data[i], data[i + 1],
  //              data[i + 2], data[i + 3], data[i + 4], data[i + 5], data[i +
  //              6], data[i + 7]);
  // }

  reserve_page(descriptor->addr << 21, page);

  lock_release(get_mem_lock());

  return (void *)(page * PAGE_SIZE + (descriptor->addr << 21));
}
