#include <hal/imemory.h>
#include <stddef.h>

void *unmap_virt(void *virt) {

  size_t virt_bits = (size_t)virt;

  PT_entry_t *entry =
      (PT_entry_t *)(PT_ADDR + sizeof(PT_entry_t) * (virt_bits / PAGE_SIZE));

  entry->full_entry = 0;

  __asm__ volatile("invlpg (%0)" : : "r"(virt_bits) : "memory");

  return virt;
}
