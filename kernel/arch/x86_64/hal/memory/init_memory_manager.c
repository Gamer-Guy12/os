#include <asm.h>
#include <hal/imemory.h>
#include <hal/memory.h>
#include <libk/bit.h>
#include <libk/err.h>
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

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t entry_size;
  uint32_t entry_version;
} __attribute__((packed)) multiboot_memory_header_t;

size_t used_page_count = 0;

extern char end_kernel[];
void *kernel_end = end_kernel;

#define NEXT_PAGE                                                              \
  (void *)((uint8_t *)ROUND_UP((size_t)kernel_end, MB * 2) +                   \
           used_page_count * PAGE_SIZE)
#define CLEAR_PAGE(ptr) memset(ptr, 0, PAGE_SIZE)
#define CLEAR_PAGES(ptr, count) memset(ptr, 0, PAGE_SIZE *count)
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
      bool before_kernel = (MB2 * i + PAGE_SIZE * j) <
                           ((size_t)kernel_start - KERNEL_CODE_OFFSET);

      pt[j].full_entry = MB2 * i + PAGE_SIZE * j;
      if (__builtin_expect(before_kernel, false)) {
        pt[j].not_executable = 0;
        pt[j].flags = PT_PRESENT;
      } else {
        pt[j].not_executable = 1;
        pt[j].flags = PT_PRESENT | PT_READ_WRITE;
      }

      pt[j].not_executable = 0;
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

static inline void create_block_descriptor(block_descriptor_t *descriptor,
                                           size_t base) {
  descriptor->free_pages = (BLOCK_SIZE / PAGE_SIZE);
  descriptor->buddy_data = NULL;
  descriptor->flags = 0;
  descriptor->addr = base >> 21;
}

/// Returns count
/// Pass in null just to get count
static size_t create_block_descriptors(block_descriptor_t *descriptors) {
  uint8_t *ptr = move_to_type(MULTIBOOT_MEMORY_MAP);
  const multiboot_memory_header_t *header = (multiboot_memory_header_t *)ptr;

  const size_t count =
      ((header->size - 16) / sizeof(multiboot_memory_t)) > MAX_BLOCK_COUNT
          ? MAX_BLOCK_COUNT
          : (header->size - 16) / sizeof(multiboot_memory_t);

  if (descriptors == NULL) {
    return count;
  }

  // Skip over the static header
  ptr += 16;
  const multiboot_memory_t *map = (multiboot_memory_t *)ptr;
  size_t block_index = 0;

  for (size_t i = 0; i < count; i++) {
    if (map[i].len < BLOCK_SIZE) {
      continue;
    }

    const uint64_t base = ROUND_UP(map[i].base, BLOCK_SIZE);
    const uint64_t len = ROUND_DOWN(map[i].len, BLOCK_SIZE);
    const uint64_t block_count = len / BLOCK_SIZE;

    if (base > (map[i].base + map[i].len)) {
      continue;
    }

    for (size_t i = 0; i < block_count; i++) {
      if (block_index >= MAX_BLOCK_COUNT)
        break;

      create_block_descriptor(&descriptors[block_index], base + i * BLOCK_SIZE);
      block_index++;
    }

    if (block_index >= MAX_BLOCK_COUNT)
      break;
  }

  return count;
}

static void create_physical_structures(void) {
  const size_t block_count = create_block_descriptors(NULL);
  const size_t pages_needed =
      ROUND_UP(block_count * sizeof(block_descriptor_t), PAGE_SIZE) / PAGE_SIZE;

  // Just to reserve the pages, will be remmapped
  block_descriptor_t *descriptors = NEXT_PAGE;
  used_page_count += pages_needed;

  // Map into virtual memory
  const size_t phys_1 = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
  used_page_count++;
  const size_t phys_2 = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
  used_page_count++;
  const size_t phys_3 = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
  used_page_count += pages_needed * PAGE_SIZE / (MB * 2);

  PML4_entry_t *pml4 = (PML4_entry_t *)PML4_ADDR;
  PDPT_entry_t *pdpt = (PDPT_entry_t *)PDPT_ADDR;
  PDT_entry_t *pdt = (PDT_entry_t *)PDT_ADDR;
  PT_entry_t *pt = (PT_entry_t *)PT_ADDR;

  pml4[256].full_entry = phys_1;
  pml4[256].not_executable = 1;
  pml4[256].flags = PML4_PRESENT | PML4_READ_WRITE;

  // Add 512 * 256 because thats how the offsets work with recursive paging
  pdpt[512 * 256 + 0].full_entry = phys_2;
  pdpt[512 * 256 + 0].not_executable = 1;
  pdpt[512 * 256 + 0].flags = PDPT_PRESENT | PDPT_READ_WRITE;

  // Make the pdts
  for (size_t i = 0; i < pages_needed * PAGE_SIZE / (MB * 2); i++) {
    // Multiply by another 512 to do some more skipping over the pdpts
    pdt[512 * 512 * 256 + i].full_entry = phys_3 + PAGE_SIZE * i;
    pdt[512 * 512 * 256 + i].not_executable = 1;
    pdt[512 * 512 * 256 + i].flags = PDT_PRESENT | PDT_READ_WRITE;
  }

  // Do skipping and looping, yay
  // Make the pts
  for (size_t i = 0; i < pages_needed; i++) {
    pt[512ull * 512ull * 512ull * 256ull + i].full_entry =
        (size_t)descriptors - KERNEL_CODE_OFFSET + PAGE_SIZE * i;
    pt[512ull * 512ull * 512ull * 256ull + i].not_executable = 1;
    pt[512ull * 512ull * 512ull * 256ull + i].flags =
        PT_PRESENT | PT_READ_WRITE;
  }

  __asm__ volatile("mov %%cr3, %%rax; mov %%rax, %%cr3" ::: "rax");

  descriptors = (block_descriptor_t *)BLOCK_DESCRIPTORS_ADDR;

  CLEAR_PAGES(descriptors, pages_needed);

  if (block_count != create_block_descriptors(descriptors)) {
    sys_panic(MULTIBOOT_ERR);
  }

  set_block_descriptor_ptr((const block_descriptor_t *)BLOCK_DESCRIPTORS_ADDR);
}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
void init_memory_manager(void) {
// Allow not executable bit
#define EXECUTE_DISABLE_BIT_ENABLE 1 << 11
  wrmsr(IA32_EFER, rdmsr(IA32_EFER) | EXECUTE_DISABLE_BIT_ENABLE);

  // Add Fmem if you want
  create_page_tables();

  void *phys_1 = (void *)(GB - PAGE_SIZE * 1);
  void *phys_2 = (void *)(GB - PAGE_SIZE * 2);
  void *phys_3 = (void *)(GB - PAGE_SIZE * 3);

  map_virt_to_phys((void *)PDPT_ADDR, phys_1, 1, PML4_READ_WRITE);
  kio_printf("Phys addr %x, virt addr %x\n", virt_to_phys(PDPT_ADDR),
             PDPT_ADDR);

  // __asm__ volatile("mov %%cr3, %%rax; mov %%rax, %%cr3" ::: "rax");
  // CLEAR_PAGE((void *)PDPT_ADDR);
  // uint64_t *ptr = (uint64_t *)PDPT_ADDR;
  // *ptr = 9;

  map_virt_to_phys((void *)PDT_ADDR, phys_2, 1, PDPT_READ_WRITE);

  kio_printf("Phys addr %x, virt addr %x\n", virt_to_phys(PDT_ADDR), PDT_ADDR);
  map_virt_to_phys((void *)PT_ADDR, phys_3, 1, PDT_READ_WRITE);
  map_virt_to_phys((void *)(PAGE_SIZE * 2), (void *)(GB - PAGE_SIZE * 4), 1,
                   PT_READ_WRITE);

  __asm__ volatile("mov %%cr3, %%rax; mov %%rax, %%cr3" ::: "rax");

  kio_printf("The mapped addr is %x\n", virt_to_phys(PAGE_SIZE * 2));

  uint64_t *ptr = (uint64_t *)(PAGE_SIZE * 2);
  *ptr = 458;

  kio_printf("Stored value is %u\n", *ptr);

  return;
  create_physical_structures();
}
