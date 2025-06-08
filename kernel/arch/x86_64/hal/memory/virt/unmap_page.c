#include "libk/kio.h"
#include <hal/memory.h>
#include <hal/pimemory.h>
#include <hal/vimemory.h>
#include <stddef.h>

#define UNCANONICALIZER 0x0000fffffffff000

inline static void check_pt(void *addr);
inline static void check_pdt(void *addr);
inline static void check_pdpt(void *addr);
inline static void check_pml4(void *addr);

void *unmap_page(void *addr) {
  phys_free(unmap_virt(addr));
  // Check if there are any other pages in the pt
  check_pt(addr);
  check_pdt(addr);
  check_pdpt(addr);
  check_pml4(addr);

  return addr;
}

inline static void check_pt(void *addr) {
  const size_t addr_bits = (size_t)addr;

  const size_t mask = 0x1ff000;

  PT_entry_t *entries = (PT_entry_t *)PT_ADDR;
  size_t index = (addr_bits & UNCANONICALIZER & mask) / PAGE_SIZE;

  for (size_t i = 0; i < 512; i++) {
    if (!(entries[index + i].full_entry & PT_PRESENT)) {
      // Delete the pt
      void *addr = (void *)(PT_ADDR + index * 8);
      void *phys = (void *)virt_to_phys((size_t)addr);

      unmap_virt(addr);
      return;
      phys_free(phys);
      kio_printf("Remove PT\n");

      return;
    }
  }
}

inline static void check_pdt(void *addr) {}

inline static void check_pdpt(void *addr) {}

inline static void check_pml4(void *addr) {}
