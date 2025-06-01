#include <hal/imemory.h>
#include <hal/memory.h>
#include <libk/kio.h>
#include <libk/lock.h>
#include <libk/math.h>
#include <stddef.h>

inline static void set_bit_in_ptr(uint8_t *ptr, size_t bit) {
  size_t index = ROUND_DOWN(bit, 8);
  size_t offset = bit - index;

  ptr[index] |= 1 << offset;
}

inline static bool check_bit_in_ptr(const uint8_t *ptr, size_t bit) {
  size_t index = ROUND_DOWN(bit, 8);
  size_t offset = bit - index;

  return ptr[index] & 1 << offset;
}

// returns the bit that is unset in the pair
// It can never be 0
inline static size_t find_pair(size_t order, block_descriptor_t *descriptor) {
  size_t start_index = math_powu64(2, BUDDY_MAX_ORDER - order) - 1;
  size_t end_index = math_powu64(2, BUDDY_MAX_ORDER - order + 1) - 1;

  for (size_t i = 0; i < end_index - start_index; i += 2) {
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

inline static void set_bit_in_layer(block_descriptor_t *descriptor,
                                    size_t order, size_t index) {
  size_t start_index = math_powu64(2, BUDDY_MAX_ORDER - order) - 1;

  set_bit_in_ptr(descriptor->buddy_data, start_index + index);
}

// returns the bit at the bottom
static size_t set_bit_structure(block_descriptor_t *descriptor, size_t order,
                                size_t bit) {
  size_t index = bit;

  for (size_t i = order; i >= 0; i--) {
    set_bit_in_layer(descriptor, i, index);

    if (i == 0) {
      return index;
    }

    find_pair(i - 1, descriptor);
  }

  return 0;
}

// Algorithm:
// Start with the bottom layer and go up looking for pairs of 10s or 01s
// Once one is found work your way down making those 10s
// If you get to the order
//
// Don't acquire lock because phys_alloc already did
// If the two children are both 1s then it has no children available
// If one child is 1 and the other is 0 that means that the 1 might have
// available space and the 0 definitly does
static inline void *phys_alloc_in_descriptor(block_descriptor_t *descriptor) {
  descriptor->free_pages--;

  for (size_t i = 0; i < BUDDY_MAX_ORDER + 1; i++) {
    if (i == BUDDY_MAX_ORDER) {
      set_bit_in_ptr(descriptor->buddy_data, 0);

      size_t bit = find_pair(i - 1, descriptor);

      size_t addr_offset = set_bit_structure(descriptor, i - 1, bit) -
                           (math_powu64(2, BUDDY_MAX_ORDER) - 1);

      if (addr_offset == 0)
        return NULL;

      return (void *)((descriptor->addr << 21) + addr_offset * PAGE_SIZE);
    }

    size_t bit = find_pair(i, descriptor);

    if (!bit)
      continue;

    size_t addr_offset = set_bit_structure(descriptor, i, bit) -
                         (math_powu64(2, BUDDY_MAX_ORDER) - 1);

    return (void *)((descriptor->addr << 21) + addr_offset * PAGE_SIZE);
  }

  return NULL;
}

void *phys_alloc(void) {
  lock_acquire(get_mem_lock());

  size_t block_count = get_block_count();
  block_descriptor_t *descriptors =
      (block_descriptor_t *)BLOCK_DESCRIPTORS_ADDR;

  for (size_t i = 0; i < block_count; i++) {
    if (descriptors[i].free_pages > 0 &&
        !(descriptors[i].flags & BLOCK_DESCRIPTOR_RESERVED)) {
      lock_release(get_mem_lock());

      return phys_alloc_in_descriptor(&descriptors[i]);
    }
  }

  lock_release(get_mem_lock());
  return NULL;
}
