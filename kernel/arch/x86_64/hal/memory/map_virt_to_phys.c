#include <hal/imemory.h>
#include <stddef.h>
#include <stdint.h>

#define UNCANONICALIZER 0x0000fffffffff000

void *map_virt_to_phys(void *virt, void *phys, bool not_executable,
                       uint16_t flags) {

  size_t virt_bits = (size_t)virt;
  size_t phys_bits = (size_t)phys;

  uint8_t small_flags = flags & 0xFF;
  small_flags |= PT_PRESENT;
  bool global = flags & PT_GLOBAL;

  PT_entry_t *entries = (PT_entry_t *)PT_ADDR;

  entries[(virt_bits & UNCANONICALIZER) / PAGE_SIZE].full_entry =
      phys_bits & UNCANONICALIZER;
  entries[(virt_bits & UNCANONICALIZER) / PAGE_SIZE].flags = small_flags;
  if (global)
    entries[(virt_bits & UNCANONICALIZER) / PAGE_SIZE].flags |= PT_GLOBAL;
  entries[(virt_bits & UNCANONICALIZER) / PAGE_SIZE].not_executable =
      not_executable;

  return virt;

  __asm__ volatile("invlpg (%0)" : : "r"(virt_bits) : "memory");

  return virt;
}
