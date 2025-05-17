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

extern char end_kernel[];
void *kernel_end = end_kernel;

size_t used_page_count = 0;

#define NEXT_PAGE                                                              \
  (void *)(ROUND_UP((size_t)(uint8_t *)kernel_end, PAGE_SIZE) +                \
           used_page_count * PAGE_SIZE)
#define CLEAR_PAGE(ptr) memset(ptr, 0, PAGE_SIZE)

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

  pml4[511].flags = PML4_USER_PAGE | PML4_READ_WRITE | PML4_PRESENT;
  pml4[511].not_executable = 0;
  pml4[511].full_entry |= PAGE_ADDR(pdpt);

  kio_printf("PML4: index 0, val %x, ptr %x\n", pml4[511].full_entry,
             (size_t)pdpt - KERNEL_OFFSET);

  // Save 2 pages (hopefully contigouous)
  // Should be cuz no other concurrent things
  PDT_entry_t *pdt = safe_fmem_push();
  safe_fmem_push();
  memset(pdt, 0, PAGE_SIZE * 2);

  pdpt[510].flags = PDPT_PRESENT | PDPT_READ_WRITE;
  pdpt[510].not_executable = 0;
  pdpt[510].full_entry |= PAGE_ADDR(pdt);

  pdpt[511].flags = PDPT_PRESENT | PDPT_READ_WRITE;
  pdpt[511].not_executable = 0;
  pdpt[511].full_entry |= PAGE_ADDR(&pdt[512]);

  kio_printf("PDPT: index 0, val %x, ptr %x\n", pdpt[510].full_entry,
             (size_t)pdt - KERNEL_OFFSET);
  kio_printf("PDPT: index 1, val %x, ptr %x\n\n", pdpt[511].full_entry,
             (size_t)&pdt[512] - KERNEL_OFFSET);

#define MB2 0x200000

  size_t mb2s_needed =
      ROUND_UP((size_t)fmem_get_ptr(NULL) - KERNEL_OFFSET, MB2) / MB2 + 1;

  kio_printf("MB2s: %x\n", mb2s_needed);

  // extern char _text_end[];
  // const void *text_end = _text_end;
  //
  // extern char _text[];
  // const void *text = _text;

  for (size_t i = 0; i < mb2s_needed; i++) {
    PT_entry_t *pt = safe_fmem_push();
    memset(pt, 0, PAGE_SIZE);

    pdt[i].flags = PDT_PRESENT | PDT_READ_WRITE;
    pdt[i].not_executable = 0;
    pdt[i].full_entry |= PAGE_ADDR(pt);

    kio_printf("PDT: index %x, val %x, ptr %x\n", i, pdt[i].full_entry,
               (size_t)pt - KERNEL_OFFSET);

    // 512 entries in a pt
    for (size_t j = 0; j < 512; j++) {
      size_t cur_addr = MB2 * i + PAGE_SIZE * j;

      pt[j].flags = PT_PRESENT;

      // TODO: add security for pages
      // if (cur_addr >= (size_t)text_end - KERNEL_OFFSET ||
      //     cur_addr < (size_t)text - KERNEL_OFFSET) {
      //   pt[j].flags |= PT_READ_WRITE;
      //   pt[j].not_executable = 1;
      // } else {
      //   pt[j].not_executable = 0;
      // }

      pt[j].not_executable = 0;
      pt[j].flags |= PT_READ_WRITE;
      pt[j].full_entry |= cur_addr & PAGE_TABLE_ENTRY_ADDR_MASK;

      if (0xf17fa8 > cur_addr && 0xf17fa8 < cur_addr + PAGE_SIZE) {
        kio_printf("Correct\n");
      }
    }
  }

  __asm__ volatile("movq %0, %%cr3"
                   :
                   : "r"((size_t)pml4 - KERNEL_OFFSET)
                   : "memory");

  kio_printf("PML4 addr %x\n", (size_t)pml4 - KERNEL_OFFSET);

  for (size_t i = 0; i < 5000; i++) {
    outb(0x43, 0x30);
    io_wait();
    outb(0x40, 0xA9);
    io_wait();
    outb(0x40, 0x4);
    io_wait();

    bool dontStop = true;
    while (dontStop) {
      outb(0x43, 0xE2);
      io_wait();
      dontStop = !check_bit(inb(0x40), 7);
    }
  }

  memset(NULL, 0, 3);
}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
void init_memory_manager(void) {
  fmem_init();

  create_physical_map();

  create_page_tables();
}
