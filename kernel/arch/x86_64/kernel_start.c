#include <decls.h>
#include <gdt.h>
#include <interrupts.h>
#include <libk/bst.h>
#include <libk/kgfx.h>
#include <libk/kio.h>
#include <libk/math.h>
#include <libk/spinlock.h>
#include <libk/string.h>
#include <mem/kheap.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <pic.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

extern void kernel_main(void);

// Code thanks to nullplan from the osdev wiki
// https://forum.osdev.org/viewtopic.php?t=57103 This code calls the global
// constructors
#pragma region Global Constructors
typedef void initfunc_t(void);
extern initfunc_t *__init_array_start[], *__init_array_end[];

static void handle_init_array(void) {
  size_t nfunc = ((uintptr_t)__init_array_end - (uintptr_t)__init_array_start) /
                 sizeof(initfunc_t *);
  for (initfunc_t **p = __init_array_start; p < __init_array_start + nfunc; p++)
    (*p)();
}
#pragma endregion

typedef struct {
  uint32_t total_size;
  uint32_t reserved;
} PACKED multiboot_header_t;

typedef struct {
  uint32_t type;
  uint32_t size;
} PACKED multiboot_tag_t;

void test_print(uint8_t *multiboot) {
  //  multiboot_header_t *header = (multiboot_header_t *)multiboot;

  // Skip over the header
  multiboot += 8;

  multiboot_tag_t *cur_tag = (multiboot_tag_t *)multiboot;

  kio_printf("Found multiboot tags: ");

  while (cur_tag->type != 0) {
    uint32_t true_size = ROUND_UP(cur_tag->size, 8);

    kio_printf("%u, ", (size_t)cur_tag->type);

    multiboot += true_size;
    cur_tag = (multiboot_tag_t *)multiboot;
  }

  kio_printf("\n");
}

void kernel_start(uint8_t *multiboot) {

  //  test_print(multiboot);

  handle_init_array();
  kio_printf("Called Global Constructors\n");
  init_multiboot(multiboot);
  kio_printf("Initialized Multiboot\n");
  init_memory_manager();
  kio_printf("Initialized Memory Manager\n");
  // // kio_clear();
  //
  // size_t phys_1 = (size_t)phys_alloc();
  // phys_free((void *)phys_1);
  // size_t phys_2 = (size_t)phys_alloc();
  // size_t phys_3 = (size_t)phys_alloc();
  // phys_free((void *)phys_2);
  // size_t phys_4 = (size_t)phys_alloc();
  // phys_free((void *)phys_3);
  // phys_free((void *)phys_4);
  //
  // extern char end_kernel[];
  //
  // kio_printf("Addr 1 %x, 2 %x, 3 %x, 4 %x diff %x\n", phys_1, phys_2, phys_3,
  //            phys_4, (size_t)(void *)end_kernel - KERNEL_CODE_OFFSET -
  //            phys_1);
  //
  // uint64_t *page = map_page((void *)GB, PT_READ_WRITE, 0);
  // *page = 48;
  // map_page((void *)(GB + PAGE_SIZE), PT_READ_WRITE, 1);
  //
  // kio_printf("Addr %x, value %x\n", (size_t)page, *page);
  // unmap_page((void *)GB);
  // unmap_page((void *)(GB + PAGE_SIZE));
  //
  // vmm_kernel_region_t region;
  // create_kernel_region(&region);
  //
  // increment_kernel_brk(&region, 0x4001);
  // uint64_t *num = increment_kernel_brk(&region, 0);
  // *num = 49;
  //
  // kio_printf("Num %u\n", *num);
  //
  // decrement_kernel_brk(&region, 0x4001);

  vmm_kernel_region_t region;
  create_kernel_region(&region);
  vmm_kernel_region_t **region_ptr = KERNEL_REGION_PTR_LOCATION;
  *region_ptr = &region;

  init_heap();
  kio_printf("Initialized the heap (kernel malloc)\n");

  create_gdt();
  kio_printf("Created the GDT\n");
  // Uncomment to make the kernel fault to show that moving the break backwards
  // unmaps the pages
  //*num = 49;
  //
  // kio_printf("Num %u\n", *num);

  // It would create interrupts otherwise
  disable_pic();
  init_interrupts();
  kio_printf("Initialized Interrupts\n");

  print_multiboot_info();

  kernel_main();
}
