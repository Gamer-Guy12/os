#include <hal/memory.h>
#include <hal/pimemory.h>
#include <libk/math.h>
#include <libk/spinlock.h>
#include <stddef.h>
#include <stdint.h>

static block_descriptor_t *get_descriptor(size_t base) {
  block_descriptor_t *blocks = (block_descriptor_t *)BLOCK_DESCRIPTORS_ADDR;

  for (size_t i = 0; i < get_block_count(); i++) {
    if (blocks[i].addr == (base >> 21)) {
      return &blocks[i];
    }
  }

  return NULL;
}

static void clear_bit_in_ptr(uint8_t *ptr, size_t bit) {
  size_t index = ROUND_DOWN(bit, 8);
  size_t offset = bit - index;
  index = index / 8;

  ptr[index] = ptr[index] & ~(1 << offset);
}

inline static bool check_bit_in_ptr(const uint8_t *ptr, size_t bit) {
  size_t index = ROUND_DOWN(bit, 8);
  size_t offset = bit - index;

  // kio_printf("Value %x and ret %x and offset %x %x\n", (size_t)ptr[index],
  //            (size_t)ptr[index] & (1 << offset), offset, 1 << offset);

  return ptr[index / 8] & (1 << offset);
}

static void free_page_index(block_descriptor_t *descriptor, size_t page_index) {
  // Start by clearing the bit at the bottom and then go on to merge up the tree
  size_t full_index = page_index + math_powu64(2, PHYS_BUDDY_MAX_ORDER) - 1;

  // Clear bottom bit
  clear_bit_in_ptr(descriptor->buddy_data, full_index);

  // Clear the bits above
  // Every index clears the bit above it
  size_t previous_bit = full_index;
  for (size_t i = 0; i < PHYS_BUDDY_MAX_ORDER; i++) {
    size_t size_by_2 = math_powu64(2, i + 1);
    bool is_lower = page_index % size_by_2 < size_by_2 / 2;

    if (is_lower && check_bit_in_ptr(descriptor->buddy_data, page_index + 1)) {
      return;
    } else if (!is_lower &&
               check_bit_in_ptr(descriptor->buddy_data, page_index - 1)) {
      return;
    }

    // if it is higher then we got this by doing 2 * + 2
    // If not we did this by doing 2 * + 1 so we should do those reversions

    if (!is_lower)
      previous_bit--;

    previous_bit--;
    previous_bit = previous_bit / 2;

    clear_bit_in_ptr(descriptor->buddy_data, previous_bit);
  }
}

void phys_free(void *addr) {
  spinlock_acquire(get_mem_lock());

  size_t addr_bits = (size_t)addr;
  size_t block_base = addr_bits & ~0x1FFFFF;
  size_t page_index = ((addr_bits - block_base) & ~0xFFF) >> 12;
  block_descriptor_t *descriptor = get_descriptor(block_base);

  free_page_index(descriptor, page_index);

  spinlock_release(get_mem_lock());
}
