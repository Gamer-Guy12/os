#include <asm.h>
#include <decls.h>
#include <libk/bit.h>
#include <libk/err.h>
#include <libk/kio.h>
#include <libk/mem.h>
#include <libk/spinlock.h>
#include <libk/sys.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

#define MULTIBOOT_MEMORY_MAP 6

// It turns out that this file rlly needs the old round up and down macros
#define ROUND_UP(num, to) ((num) + ((to) - ((num) % (to))))
#define ROUND_DOWN(num, to) ((num) - ((num) % (to)))

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
  (void *)((uint8_t *)(size_t)kernel_end + used_page_count * PAGE_SIZE)
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
}

static inline void create_block_descriptor(block_descriptor_t *descriptor,
                                           size_t base) {
  descriptor->free_pages = (PHYS_BLOCK_SIZE / PAGE_SIZE);
  descriptor->largest_region_order = PHYS_BUDDY_MAX_ORDER;
  descriptor->buddy_data = NULL;
  descriptor->flags = 0;
  descriptor->addr = base >> 21;
}

/// Returns count of block descriptors needed
/// Pass in null just to get count
static size_t create_block_descriptors(block_descriptor_t *descriptors) {
  uint8_t *ptr = move_to_type(MULTIBOOT_MEMORY_MAP);
  const multiboot_memory_header_t *header = (multiboot_memory_header_t *)ptr;

  const size_t entry_count =
      ((header->size - 16) / sizeof(multiboot_memory_t)) > MAX_BLOCK_COUNT
          ? MAX_BLOCK_COUNT
          : (header->size - 16) / sizeof(multiboot_memory_t);

  // Skip over the static header
  ptr += 16;
  const multiboot_memory_t *map = (multiboot_memory_t *)ptr;
  size_t block_index = 0;

  size_t block_count = 0;

  for (size_t i = 0; i < entry_count; i++) {
    if (map[i].len < PHYS_BLOCK_SIZE) {
      continue;
    }

    if (map[i].type != 1)
      continue;

    const uint64_t len = ROUND_DOWN(map[i].len, PHYS_BLOCK_SIZE);
    const uint64_t cur_count = len / PHYS_BLOCK_SIZE;

    block_count += cur_count;
  }

  if (descriptors == NULL) {
    return block_count;
  }

  for (size_t i = 0; i < entry_count; i++) {
    if (map[i].len < PHYS_BLOCK_SIZE) {
      continue;
    }

    if (map[i].type != 1)
      continue;

    const uint64_t base = ROUND_UP(map[i].base, PHYS_BLOCK_SIZE);
    const uint64_t len = ROUND_DOWN(map[i].len, PHYS_BLOCK_SIZE);
    const uint64_t block_count = len / PHYS_BLOCK_SIZE;

    if (block_count == 0)
      continue;

    if (base > (map[i].base + map[i].len)) {
      continue;
    }

    for (size_t i = 0; i < block_count; i++) {
      if (block_index >= MAX_BLOCK_COUNT)
        break;

      create_block_descriptor(&descriptors[block_index],
                              base + i * PHYS_BLOCK_SIZE);
      block_index++;
    }

    if (block_index >= MAX_BLOCK_COUNT)
      break;
  }

  return block_count;
}

static void create_block_page_tables(const size_t pages_needed) {
  // To calculate the address needed to map the 256th pdpt entry, we must do
  // some shit
  // First we start with the PDPT addr thingy. then we add nothing cuz
  // i dont think so
  // Wait no we do. And then we add 256 pages and map that to something (A
  // physical Addr)
  const size_t pdpt_phys_addr = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
  used_page_count++;
  map_virt_to_phys((void *)(PDPT_ADDR + PAGE_SIZE * 256),
                   (void *)pdpt_phys_addr, 1, PML4_READ_WRITE);
  CLEAR_PAGE((void *)(PDPT_ADDR + PAGE_SIZE * 256));

  // Each PDPT points to a PDT which can hold 1gb
  // Each descriptor is 16 bytes and holds 2 Mb
  // This is perfect for the 512 gb max memory
  // 1 PDT

  const size_t pdt_phys_addr = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
  used_page_count++;
  map_virt_to_phys((void *)(PDT_ADDR + 512 * 256 * PAGE_SIZE),
                   (void *)pdt_phys_addr, 1, PDPT_READ_WRITE);
  CLEAR_PAGE((void *)(PDT_ADDR + 512 * 256 * PAGE_SIZE));

  // Calculate PT count
  // Each one takes 2MB
  const size_t PT_count = ROUND_UP(pages_needed, (MB * 2)) / (MB * 2);
  const size_t pt_phys_addr = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
  used_page_count += PT_count;

  for (size_t i = 0; i < PT_count; i++) {
    map_virt_to_phys((void *)(PT_ADDR + 512ull * 512ull * 256ull * PAGE_SIZE +
                              PAGE_SIZE * i),
                     (void *)(pt_phys_addr + PAGE_SIZE * i), 1, PDT_READ_WRITE);
    CLEAR_PAGE((void *)(PT_ADDR + 512ull * 512ull * 256ull * PAGE_SIZE +
                        PAGE_SIZE * i));
  }

  const size_t descriptor_phys_addr = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
  used_page_count += pages_needed;

  for (size_t i = 0; i < pages_needed; i++) {
    map_virt_to_phys((void *)(KERNEL_OFFSET + PAGE_SIZE * i),
                     (void *)(descriptor_phys_addr + PAGE_SIZE * i), 1,
                     PT_READ_WRITE);
    CLEAR_PAGE((void *)(KERNEL_OFFSET + PAGE_SIZE * i));
  }
}

/// Don't worry about page alinging the thing just put it in one
static void map_multiboot(void) {
  // Get multiboot returns the physical address
  // Map the page into the 258 pdt and then should be fine
  const size_t page_offset = get_used_258_pdt_page_count();

  // Do PDPT
  if (page_offset == 0) {
    const size_t pdpt_phys = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
    used_page_count++;
    map_virt_to_phys((void *)(PDPT_ADDR + 258 * PAGE_SIZE), (void *)pdpt_phys,
                     1, PML4_READ_WRITE);
    CLEAR_PAGE((void *)(PDPT_ADDR + 258 * PAGE_SIZE));
  }

  // if gb aligned so I don't have to make the pdt
  if (page_offset % GB == 0) {
    const size_t pdt_used_count = page_offset / GB;

    const size_t pdt_phys = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
    used_page_count++;
    // I hope this skippping works
    const size_t pdt_virt =
        PDT_ADDR + 512 * 258 * PAGE_SIZE + pdt_used_count * PAGE_SIZE;
    map_virt_to_phys((void *)pdt_virt, (void *)pdt_phys, 1, PDPT_READ_WRITE);
    CLEAR_PAGE((void *)pdt_virt);
  }

  // stuff
  const size_t pdt_used_count = ROUND_DOWN(page_offset, GB) / GB;

  // if 2mb aligned so I don't have to make the pt
  if (page_offset % (MB * 2) == 0) {
    // I love these like MB GB and KB macros
    // Life so much better
    const size_t pt_used_count = page_offset / (MB * 2);

    const size_t pt_phys = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
    used_page_count++;
    // I hope THIS skipping works
    const size_t pt_virt = PT_ADDR + 512ull * 512 * 258 * PAGE_SIZE +
                           pdt_used_count * 512 * PAGE_SIZE +
                           pt_used_count * PAGE_SIZE;
    map_virt_to_phys((void *)pt_virt, (void *)pt_phys, 1, PDT_READ_WRITE);
    CLEAR_PAGE((void *)pt_virt);
  }

  // more stuff
  // yk what round down and round up are also like insanely nice macros
  // prolly inefficinet though
  const size_t pt_used_count = ROUND_DOWN(page_offset, MB * 2) / (MB * 2);

  // Map the pages

  // IDK i think thats how it works
  // Nearly a name clash
  const size_t used_page_count_offset =
      page_offset - ROUND_DOWN(page_offset, 512);

  const size_t page_phys = ROUND_DOWN((size_t)get_multiboot(), PAGE_SIZE);
  const size_t multiboot_offset = (size_t)get_multiboot() % PAGE_SIZE;
  // Skipping hell
  const size_t page_virt = 0x000 + (used_page_count_offset << 12) +
                           (pt_used_count << 21) + (pdt_used_count << 30) +
                           (258ull << 39ull) +
                           /* Cananocalization */ (0xFFFFull << 48);

  map_virt_to_phys((void *)page_virt, (void *)page_phys, 1, PT_READ_WRITE);
  reserve_258_pdt_page(1);

  uint8_t *multiboot = (uint8_t *)(page_virt + multiboot_offset);
  init_multiboot(multiboot);
  uint32_t size = *(uint32_t *)multiboot;

  // Check if the thingy needs more pages
  if (multiboot_offset + size > PAGE_SIZE) {
    const size_t new_pages_needed =
        ROUND_UP((multiboot_offset + size - PAGE_SIZE), PAGE_SIZE) / PAGE_SIZE;

    // Plus 1 cuz we forgor about the next one cuz like next one cuz like - PAGE
    // Size
    if (new_pages_needed > 512 - used_page_count_offset + 1) {
      sys_panic(MULTIBOOT_ERR | TOO_MUCH_SPACE_ERR);
    }

    // I hope this is right
    for (size_t i = 0; i < new_pages_needed; i++) {
      const size_t page_phys =
          ROUND_DOWN((size_t)get_multiboot(), PAGE_SIZE) + i + 1;

      const size_t page_virt =
          0x000 + ((used_page_count_offset + i + 1) << 12) +
          (pt_used_count << 21) + (pdt_used_count << 30) + (258ull << 39ull) +
          /* Cananocalization */ (0xFFFFull << 48);

      map_virt_to_phys((void *)page_virt, (void *)page_phys, 1, PT_READ_WRITE);
    }
  }
}

static void create_all_block_descriptors(void) {
  map_multiboot();

  const size_t block_count = create_block_descriptors(NULL);

  set_block_count(block_count);
  const size_t pages_needed =
      ROUND_UP(sizeof(block_descriptor_t) * block_count, PAGE_SIZE) / PAGE_SIZE;

  create_block_page_tables(pages_needed);

  // Wanna make sure it didn't change
  if (block_count !=
      create_block_descriptors((block_descriptor_t *)BLOCK_DESCRIPTORS_ADDR)) {
    sys_panic(MULTIBOOT_ERR);
  }

  set_block_descriptor_ptr((const block_descriptor_t *)BLOCK_DESCRIPTORS_ADDR);
}

inline void print_addr_comps(size_t addr) {

  kio_printf("pml4 %u, pdpt %u, pdt %u, pt %u, index %u\n",
             (addr >> 39) & 0x1FF, (addr >> 30) & 0x1FF, (addr >> 21) & 0x1FF,
             (addr >> 12) & 0x1FF, addr & 0xFFF);
}

static void map_buddy_memory(void) {
  // The PDPT has already been created so what is left to create are the pdts
  // and pts
  // As of now one pdt has been made but I'll just keep track (actually I won't)

  const size_t block_count = get_block_count();
  // kio_printf("Block Count: %x\n", block_count);

  const size_t bytes_taken = math_powu64(2, PHYS_BUDDY_MAX_ORDER) * 2 / 8;

  kio_printf("Block Count %x\n", block_count);

  for (size_t i = 0; i < block_count; i++) {
    bool make_pdt = ((i * bytes_taken) % (PAGE_SIZE * 512 * 512)) == 0;
    bool make_pt = ((i * bytes_taken) % (PAGE_SIZE * 512)) == 0;
    bool make_page = ((i * bytes_taken) % (PAGE_SIZE)) == 0;

    // + 1 skips over the first pdt used for the block descriptors
    size_t pdt_count = ROUND_UP(i * bytes_taken, PAGE_SIZE * 512 * 512) /
                       (PAGE_SIZE * 512 * 512);

    if (make_pdt) {
      size_t phys = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
      used_page_count++;

      size_t virt = PDT_ADDR + 512 * 256 * PAGE_SIZE + pdt_count * PAGE_SIZE;

      map_virt_to_phys((void *)virt, (void *)phys, 1, PDPT_READ_WRITE);

      CLEAR_PAGE((void *)virt);
    }

    size_t pt_count =
        ROUND_UP(i * bytes_taken, PAGE_SIZE * 512) / (PAGE_SIZE * 512);

    if (make_pt) {
      size_t phys = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
      used_page_count++;

      size_t virt = PT_ADDR + 512ull * 512 * 256 * PAGE_SIZE +
                    pdt_count * 512 * PAGE_SIZE + PAGE_SIZE * (pt_count % 512);

      map_virt_to_phys((void *)virt, (void *)phys, 1, PDT_READ_WRITE);

      CLEAR_PAGE((void *)virt);
    }

    size_t page_count = ROUND_UP(i * bytes_taken, PAGE_SIZE) / PAGE_SIZE;

    if (make_page) {
      size_t phys = (size_t)NEXT_PAGE - KERNEL_CODE_OFFSET;
      used_page_count++;

      size_t virt = INDICES_TO_ADDR(page_count, pt_count, pdt_count, 256ull);

      map_virt_to_phys((void *)virt, (void *)phys, 1, PT_READ_WRITE);

      CLEAR_PAGE((void *)virt);
    }
  }
}

static void modify_descriptors(void) {
  const size_t block_descriptor_count = get_block_count();

  block_descriptor_t *descriptors =
      (block_descriptor_t *)BLOCK_DESCRIPTORS_ADDR;

  size_t buddy_ptr = BLOCK_DESCRIPTORS_ADDR + PAGE_SIZE;

  const size_t bytes_taken = math_powu64(2, PHYS_BUDDY_MAX_ORDER) * 2 / 8;

  for (size_t i = 0; i < block_descriptor_count; i++) {
    descriptors[i].buddy_data = (void *)(buddy_ptr + bytes_taken * i);
  }
}

static void create_physical_structures(void) {
  create_all_block_descriptors();

  map_buddy_memory();

  modify_descriptors();
}

inline static void set_bit_in_ptr(uint8_t *ptr, size_t bit) {
  size_t index = ROUND_DOWN(bit, 8);
  size_t offset = bit - index;

  ptr[index / 8] |= 1 << offset;
}

inline static bool check_bit_in_ptr(const uint8_t *ptr, size_t bit) {
  size_t index = ROUND_DOWN(bit, 8);
  size_t offset = bit - index;

  // kio_printf("Value %x and ret %x and offset %x %x\n", (size_t)ptr[index],
  //            (size_t)ptr[index] & (1 << offset), offset, 1 << offset);

  return ptr[index / 8] & (1 << offset);
}

inline static bool block_of_order_exists(const uint8_t *ptr, size_t order) {
  size_t start_index = math_powu64(2, PHYS_BUDDY_MAX_ORDER - order) - 1;
  size_t end_index = math_powu64(2, PHYS_BUDDY_MAX_ORDER - order + 1) - 1;

  for (size_t i = 0; i < end_index - start_index; i++) {
    if (check_bit_in_ptr(ptr, start_index + i)) {
      return true;
    }
  }

  return false;
}

static block_descriptor_t *get_descriptor(size_t base) {
  block_descriptor_t *blocks = (block_descriptor_t *)BLOCK_DESCRIPTORS_ADDR;

  for (size_t i = 0; i < get_block_count(); i++) {
    if (blocks[i].addr == (base >> 21)) {
      return &blocks[i];
    }
  }

  return NULL;
}

static void reserve_block(size_t base) {

  block_descriptor_t *blocks = (block_descriptor_t *)BLOCK_DESCRIPTORS_ADDR;

  for (size_t i = 0; i < get_block_count(); i++) {
    if (blocks[i].addr == base >> 21) {
      blocks[i].flags |= BLOCK_DESCRIPTOR_RESERVED;
      blocks[i].free_pages = 0;
      blocks[i].largest_region_order = 0;
    }
  }
}

static void reserve_page(size_t block_base, size_t page) {
  block_descriptor_t *descriptor = get_descriptor(block_base);

  if (descriptor == NULL) {
    sys_panic(MEMORY_INIT_ERR | MISSING_BLOCK_ERR);
  }

  descriptor->free_pages--;

  uint8_t *data = descriptor->buddy_data;

  // Top level block is destroyed
  size_t previous_bit = 0;

  for (size_t i = PHYS_BUDDY_MAX_ORDER; i > 0; i--) {
    if (i == PHYS_BUDDY_MAX_ORDER) {
      data[0] |= 1;
      continue;
    }
    // Its actually 2 times the size
    size_t size = math_powu64(2, i + 1);
    bool is_lower = page % size < size / 2;

    if (!is_lower) {
      previous_bit = previous_bit * 2 + 2;
      set_bit_in_ptr(data, previous_bit);
    } else {
      previous_bit = previous_bit * 2 + 1;
      set_bit_in_ptr(data, previous_bit);
    }
  }

  // For the bottom layer
  if (page % 2 == 0) {
    previous_bit = previous_bit * 2 + 1;
    set_bit_in_ptr(data, previous_bit);
  } else {
    previous_bit = previous_bit * 2 + 2;
    set_bit_in_ptr(data, previous_bit);
  }

  for (size_t i = PHYS_BUDDY_MAX_ORDER; i > 0; i++) {
    if (block_of_order_exists(data, i)) {
      descriptor->largest_region_order = i;
      return;
    }
  }

  descriptor->largest_region_order = 0;
}

static bool check_page_index(block_descriptor_t *descriptor, size_t page) {
  uint8_t *data = descriptor->buddy_data;

  size_t prev_bit = 0;

  for (size_t i = PHYS_BUDDY_MAX_ORDER; i > 0; i--) {
    if (i == PHYS_BUDDY_MAX_ORDER) {
      if (!(data[0] & 1)) {
        // kio_printf("Index %x\n", i);
        return false;
      }
    } else {
      // 2 * size
      size_t size = math_powu64(2, i + 1);
      bool is_lower = page % size < size / 2;

      if (!is_lower) {
        prev_bit = prev_bit * 2 + 2;
        if (!(check_bit_in_ptr(data, prev_bit))) {
          // kio_printf("Index %x %x\n", i, prev_bit);
          return false;
        }
        // kio_printf("Lower %x  ", prev_bit);
      } else {
        prev_bit = prev_bit * 2 + 1;
        if (!(check_bit_in_ptr(data, prev_bit))) {
          // kio_printf("Idex %x %x\n", i, prev_bit);
          return false;
        }
        // kio_printf("Higher %x  ", prev_bit);
      }
    }
  }
  // kio_printf("\n");

  if (page % 2 == 0) {
    prev_bit = prev_bit * 2 + 1;
    if (!check_bit_in_ptr(data, prev_bit)) {
      // kio_printf("Inde %x\n", prev_bit);
      return false;
    }
  } else {
    prev_bit = prev_bit * 2 + 2;
    if (!(check_bit_in_ptr(data, prev_bit))) {
      // kio_printf("Ide %x\n", prev_bit);
      return false;
    }
  }

  return true;
}

static void reserve_kernel_structures(void) {
  extern char end_kernel[];
  const size_t kernel_end = (size_t)(void *)end_kernel;
  const size_t kernel_block_count =
      (kernel_end - KERNEL_CODE_OFFSET) / PHYS_BLOCK_SIZE;

  for (size_t i = 0; i < kernel_block_count; i++) {
    reserve_block(i * PHYS_BLOCK_SIZE);
  }

  size_t prev_base = 0;

  for (size_t i = 0; i < used_page_count; i++) {
    size_t block_base = ROUND_DOWN(i, BLOCKS_PER_PAGE) * PHYS_BLOCK_SIZE +
                        kernel_block_count * PHYS_BLOCK_SIZE;

    if (prev_base != block_base) {
      block_descriptor_t *descriptor = get_descriptor(block_base);
      memset(descriptor->buddy_data, 0,
             math_powu64(2, PHYS_BUDDY_MAX_ORDER + 1));
      prev_base = block_base;
    }

    size_t page = i % BLOCKS_PER_PAGE;

    reserve_page(block_base, page);
  }
  uint8_t *data =
      get_descriptor(kernel_block_count * PHYS_BLOCK_SIZE)->buddy_data;

  for (size_t i = 0; i < 128; i += 8) {
    kio_printf("Data: %x %x %x %x %x %x %x %x\n", data[i], data[i + 1],
               data[i + 2], data[i + 3], data[i + 4], data[i + 5], data[i + 6],
               data[i + 7]);
  }

  for (size_t i = 0; i < used_page_count; i++) {
    if (!check_page_index(get_descriptor(kernel_block_count * PHYS_BLOCK_SIZE),
                          i)) {
      kio_printf("Page %x is %x\n", i,
                 (size_t)check_page_index(
                     get_descriptor(kernel_block_count * PHYS_BLOCK_SIZE), i));
    }
  }
}

/// This function cannot call mmap or physical map or anything cuz like they
/// depend on it being ready
void init_memory_manager(void) {
// Allow not executable bit
#define EXECUTE_DISABLE_BIT_ENABLE 1 << 11

  wrmsr(IA32_EFER, rdmsr(IA32_EFER) | EXECUTE_DISABLE_BIT_ENABLE);

  // Add Fmem if you want
  create_page_tables();

  create_physical_structures();

  reserve_kernel_structures();

  extern char end_kernel[];
  size_t usage = (size_t)(void *)end_kernel - KERNEL_CODE_OFFSET +
                 (used_page_count - 1) * PAGE_SIZE;

  kio_printf("Total usage %x\n", usage);
}
