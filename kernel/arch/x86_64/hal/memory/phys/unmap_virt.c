#include <hal/pimemory.h>
#include <stddef.h>

#define UNCANONICALIZER 0x0000fffffffff000

/// Returns the address of its physical page
void *unmap_virt(void *virt) {

  size_t virt_bits = (size_t)virt;

  size_t ret = virt_to_phys(virt_bits);

  PT_entry_t *entry =
      (PT_entry_t *)(PT_ADDR + sizeof(PT_entry_t) *
                                   ((virt_bits & UNCANONICALIZER) / PAGE_SIZE));
  entry->full_entry = 0;

  __asm__ volatile("invlpg (%0)" : : "r"(virt_bits) : "memory");

  return (void *)ret;
}
