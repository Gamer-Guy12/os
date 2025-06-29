#include <libk/kio.h>
#include <mem/pimemory.h>
#include <stddef.h>
#include <stdint.h>

#define UNCANONICALIZER 0x0000fffffffff000
#define PHYS_MASK 0x0007fffffffff000

void *map_virt_to_phys(void *virt, void *phys, bool not_executable,
                       uint16_t flags) {
  size_t virt_bits = (size_t)virt;
  size_t phys_bits = (size_t)phys;

  uint8_t bit8_flags = flags & 0xFF;

  PT_entry_t *entries = (PT_entry_t *)PT_ADDR;
  // Get the address and page index
  size_t index = (virt_bits & UNCANONICALIZER) / PAGE_SIZE;

  // kio_printf("Calculated address is %x\n", PT_ADDR + index * 8);

  entries[index].full_entry = 0;
  entries[index].full_entry = (phys_bits & PHYS_MASK) & ~(1ull << 51);
  entries[index].flags = bit8_flags | PT_PRESENT;
  if (flags & PT_GLOBAL)
    entries[index].flags |= PT_GLOBAL;
  entries[index].not_executable = not_executable;

  // kio_printf("The entry created is this %x\n", entries[index].full_entry);

  __asm__ volatile("invlpg (%0)" : : "r"(virt_bits) : "memory");

  return virt;
}
