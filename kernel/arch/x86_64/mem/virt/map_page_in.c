#include <libk/mem.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stddef.h>
#include <stdint.h>

#define UNCANONICALIZER 0x0000fffffffff000

// Returns the physical pointer
static size_t do_pml4(PML4_entry_t *pml4, size_t index) {

  if (!(pml4[index].flags & PML4_PRESENT)) {

    size_t ptr = (size_t)phys_alloc();

    pml4[index].full_entry = ptr;
    pml4[index].not_executable = 0;
    pml4[index].flags |= PML4_PRESENT | PML4_READ_WRITE | PML4_USER_PAGE;

    memset((void *)(ptr + IDENTITY_MAPPED_ADDR), 0, PAGE_SIZE);
  }

  return (pml4[index].full_entry & UNCANONICALIZER);
}

static size_t do_pdpt(PDPT_entry_t *pdpt, size_t index) {
  if (!(pdpt[index].flags & PDPT_PRESENT)) {
    size_t ptr = (size_t)phys_alloc();

    pdpt[index].full_entry = ptr;
    pdpt[index].not_executable = 0;
    pdpt[index].flags |= PDPT_PRESENT | PDPT_READ_WRITE | PDPT_USER_PAGE;

    memset((void *)(ptr + IDENTITY_MAPPED_ADDR), 0, PAGE_SIZE);
  }

  return (pdpt[index].full_entry & UNCANONICALIZER);
}

static size_t do_pdt(PDT_entry_t *pdt, size_t index) {
  if (!(pdt[index].flags & PDT_PRESENT)) {
    size_t ptr = (size_t)phys_alloc();

    pdt[index].full_entry = ptr;
    pdt[index].not_executable = 0;
    pdt[index].flags |= PDT_PRESENT | PDT_READ_WRITE | PDT_USER_PAGE;
  }

  return (pdt[index].full_entry & UNCANONICALIZER);
}

static void do_page(PT_entry_t *pt, size_t index, uint16_t flags,
                    bool not_executable) {
  size_t ptr = (size_t)phys_alloc();

  pt[index].full_entry = ptr;
  pt[index].flags = flags;
  pt[index].not_executable = not_executable;
}

void *map_page_in(void *addr, uint16_t flags, bool not_executable,
                  PML4_entry_t *pml4) {
  const size_t addr_bits = (size_t)addr & UNCANONICALIZER;

  const size_t pt_index = (addr_bits >> 12) & 0x1ff;
  const size_t pdt_index = (addr_bits >> 21) & 0x1ff;
  const size_t pdpt_index = (addr_bits >> 30) & 0x1ff;
  const size_t pml4_index = (addr_bits >> 39) & 0x1ff;

  const size_t pdpt = do_pml4(pml4, pml4_index);
  const size_t pdt =
      do_pdpt((PDPT_entry_t *)(pdpt + IDENTITY_MAPPED_ADDR), pdpt_index);
  const size_t pt =
      do_pdt((PDT_entry_t *)(pdt + IDENTITY_MAPPED_ADDR), pdt_index);

  do_page((void *)(pt + IDENTITY_MAPPED_ADDR), pt_index, flags, not_executable);

  return addr;
}
