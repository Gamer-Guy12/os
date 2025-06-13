#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stddef.h>

#define UNCANONICALIZER 0x0000fffffffff000

static bool check_page(void *addr) {
  PT_entry_t *entries = (PT_entry_t *)PT_ADDR;
  // Get the address and page index
  size_t index = ((size_t)addr & UNCANONICALIZER) / PAGE_SIZE;

  return entries[index].full_entry & PT_PRESENT;
}

static void *map_pml4(size_t index) {
  size_t map_index = PDPT_ADDR + index * PAGE_SIZE;
  if (check_page((void *)map_index))
    return (void *)map_index;

  void *phys = phys_alloc();

  /// These flags are here because they mean that no matter what it can be
  /// allocated
  /// At the bottom actual flags are made
  return map_virt_to_phys((void *)map_index, phys, 0,
                          PML4_PRESENT | PML4_READ_WRITE | PML4_USER_PAGE);
}

static void *map_pdpt(size_t index, size_t pml4) {
  size_t map_index = PDT_ADDR + index * PAGE_SIZE + pml4 * 512 * PAGE_SIZE;
  if (check_page((void *)map_index))
    return (void *)map_index;

  void *phys = phys_alloc();

  /// These flags are here because they mean that no matter what it can be
  /// allocated
  /// At the bottom actual flags are made
  return map_virt_to_phys((void *)map_index, phys, 0,
                          PDPT_PRESENT | PDPT_READ_WRITE | PDPT_USER_PAGE);
}

static void *map_pdt(size_t index, size_t pdpt, size_t pml4) {
  size_t map_index = PT_ADDR + index * PAGE_SIZE + pdpt * 512 * PAGE_SIZE +
                     pml4 * 512 * 512 * PAGE_SIZE;
  if (check_page((void *)map_index)) {
    // kio_printf("Leave\n");
    return (void *)map_index;
  }

  void *phys = phys_alloc();

  /// These flags are here because they mean that no matter what it can be
  /// allocated
  /// At the bottom actual flags are made
  return map_virt_to_phys((void *)map_index, phys, 0,
                          PDT_PRESENT | PDT_READ_WRITE | PDT_USER_PAGE);
}

void *map_page(void *addr, uint16_t flags, bool not_executable) {
  size_t addr_bits = (size_t)addr;

  size_t pdt_index = (addr_bits >> 21) & 0x1ff;
  size_t pdpt_index = (addr_bits >> 30) & 0x1ff;
  size_t pml4_index = (addr_bits >> 39) & 0x1ff;

  map_pml4(pml4_index);
  map_pdpt(pdpt_index, pml4_index);
  map_pdt(pdt_index, pdpt_index, pml4_index);

  void *phys = phys_alloc();

  map_virt_to_phys(addr, phys, not_executable, flags | PT_PRESENT);

  return addr;
}
