#include <asm.h>
#include <hal/imemory.h>
#include <hal/memory.h>
#include <libk/bit.h>
#include <libk/kio.h>
#include <libk/lock.h>
#include <libk/math.h>
#include <libk/mem.h>
#include <libk/sys.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

#define MULTIBOOT_MEMORY_MAP 6

typedef struct {
  uint64_t base;
  uint64_t len;
  uint32_t type;
  uint32_t reserved;
} __attribute__((packed)) multiboot_memory_t;

size_t used_page_count = 0;

extern char end_kernel[];
void *kernel_end = end_kernel;

#define NEXT_PAGE (void *)((uint8_t *)kernel_end + used_page_count * PAGE_SIZE)
#define CLEAR_PAGE(ptr) memset(ptr, 0, PAGE_SIZE)
#define PAGE_ADDR(ptr) (((size_t)ptr - KERNEL_CODE_OFFSET) & 0x0007fffffffff000)

size_t pml4_phys = 0;

static void create_page_tables(void) {
  // The page tables will be after the kernel but will then be remapped to be
  // recursive page tables

  PML4_entry_t *pml4 = NEXT_PAGE;
  used_page_count++;
  CLEAR_PAGE(pml4);

  PDPT_entry_t *pdpt = NEXT_PAGE;
  used_page_count++;
  CLEAR_PAGE(pdpt);

  kio_printf("Address: %x and %x\n", (size_t)pml4 - KERNEL_CODE_OFFSET,
             (size_t)pdpt - KERNEL_CODE_OFFSET);

  pml4[511].full_entry = PAGE_ADDR(pdpt);
  pml4[511].not_executable = 0;
  pml4[511].flags = PML4_PRESENT | PML4_READ_WRITE;

  PDT_entry_t *pdt = NEXT_PAGE;
  used_page_count += 2;
  CLEAR_PAGE(pdt);
  CLEAR_PAGE(&pdt[512]);

  pdpt[510].full_entry = PAGE_ADDR(pdt);
  pdpt[510].not_executable = 0;
  pdpt[510].flags = PDPT_PRESENT | PDPT_READ_WRITE;

  pdpt[511].full_entry = PAGE_ADDR(&pdt[512]);
  pdpt[511].not_executable = 0;
  pdpt[511].flags = PDPT_PRESENT | PDPT_READ_WRITE;

#define MB2 0x200000

  extern char start_kernel[];
  void *kernel_start = start_kernel;

  size_t mb2s_needed =
      ROUND_UP((size_t)kernel_end - KERNEL_CODE_OFFSET, MB2) / MB2;

  for (size_t i = 0; i < mb2s_needed; i++) {
    PT_entry_t *pt = NEXT_PAGE;
    used_page_count++;
    CLEAR_PAGE(pt);

    kio_printf("Addresses: %x, %x and the pt %x\n",
               (size_t)pdt - KERNEL_CODE_OFFSET,
               (size_t)&pdt[512] - KERNEL_CODE_OFFSET,
               (size_t)pt - KERNEL_CODE_OFFSET);

    pdt[i].full_entry = PAGE_ADDR(pt);
    pdt[i].not_executable = 0;
    pdt[i].flags = PDT_PRESENT | PDT_READ_WRITE;

    for (size_t j = 0; j < 512; j++) {
      // The bios stuff is also there so ja
      bool before_kernel =
          MB2 * i + PAGE_SIZE * j < (size_t)kernel_start - KERNEL_CODE_OFFSET;

      pt[j].full_entry = MB2 * i + PAGE_SIZE * j;
      if (__builtin_expect(before_kernel, false)) {
        pt[j].not_executable = 0;
        pt[j].flags = PT_PRESENT;
      } else {
        pt[j].not_executable = 1;
        pt[j].flags = PT_PRESENT | PT_READ_WRITE;
      }

      pt[j].not_executable = 0;
      pt[j].flags = PT_PRESENT | PT_READ_WRITE;
    }
  }

  // Recursive Paging
  PT_entry_t *recursive_entry = (PT_entry_t *)&pml4[510];
  recursive_entry->full_entry = PAGE_ADDR((size_t)pml4);
  recursive_entry->not_executable = 0;
  recursive_entry->flags = PT_PRESENT | PT_READ_WRITE | PT_GLOBAL;

  pml4_phys = (size_t)pml4 - KERNEL_CODE_OFFSET;

  __asm__ volatile("mov %0, %%cr3"
                   :
                   : "r"((size_t)pml4 - KERNEL_CODE_OFFSET)
                   : "memory");

  kio_printf("ADDR: %x\n", (size_t)pml4 - KERNEL_CODE_OFFSET);
}

inline size_t virt_to_phys(size_t addr) {
  PT_entry_t *pt = (PT_entry_t *)PT_ADDR;

  size_t pml4_index = (addr >> 39) & 0x1FF;
  size_t pdpt_index = (addr >> 30) & 0x1FF;
  size_t pdt_index = (addr >> 21) & 0x1FF;
  size_t pt_index = (addr >> 12) & 0x1FF;

  PT_entry_t entry = pt[pt_index + pdt_index * 512 + pdpt_index * 512 * 512 +
                        pml4_index * 512 * 512 * 512];

  return entry.full_entry & PAGE_TABLE_ENTRY_ADDR_MASK;
}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
void init_memory_manager(void) {
  // Add Fmem if you want
  create_page_tables();

  PML4_entry_t *pml4 = (PML4_entry_t *)PML4_ADDR;
  PDPT_entry_t *pdpt = (PDPT_entry_t *)PDPT_ADDR;
  PDT_entry_t *pdt = (PDT_entry_t *)PDT_ADDR;
  PT_entry_t *pt = (PT_entry_t *)PT_ADDR;

  pml4[0].full_entry = GB + PAGE_SIZE * 1;
  pml4[0].not_executable = 0;
  pml4[0].flags = PML4_READ_WRITE | PML4_PRESENT;

  pdpt[0].full_entry = GB + PAGE_SIZE * 2;
  pdpt[0].not_executable = 0;
  pdpt[0].flags = PDPT_PRESENT | PDPT_READ_WRITE;

  pdt[0].full_entry = GB + PAGE_SIZE * 3;
  pdt[0].not_executable = 0;
  pdt[0].flags = PDT_PRESENT | PDT_READ_WRITE;

  pt[511].full_entry = (size_t)GB;
  pt[511].not_executable = 0;
  pt[511].flags = PT_PRESENT | PT_READ_WRITE;

  __asm__ volatile("mov %%cr3, %%rax; mov %%rax, %%cr3" : : : "rax");

  kio_printf("pml4[0], %x\n", pml4[0].full_entry);
  kio_printf("pdpt[0], %x\n", pdpt[0].full_entry);
  kio_printf("pdt[0], %x\n", pdt[0].full_entry);
  kio_printf("pt[0], %x\n", pt[0].full_entry);
  kio_printf("pml4 addr: %x\n", virt_to_phys(PML4_ADDR));
  kio_printf("pdpt addr: %x\n", virt_to_phys(PDPT_ADDR));
  kio_printf("pdt addr: %x\n", virt_to_phys(PDT_ADDR));
  kio_printf("pt addr: %x\n", virt_to_phys(PT_ADDR));
  kio_printf("raw addr: %x\n", virt_to_phys(0x0));
  kio_printf("GB: %x\n", (size_t)GB);

  uint64_t *addr = (uint64_t *)(PAGE_SIZE * 511);

  *addr = 49;

  kio_printf("Value stored: %x\n", *addr);
}
