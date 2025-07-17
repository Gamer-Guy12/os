#include <acpi/acpi.h>
#include <apic.h>
#include <asm.h>
#include <cls.h>
#include <decls.h>
#include <gdt.h>
#include <hal/hal.h>
#include <hal/irq.h>
#include <interrupts.h>
#include <libk/kgfx.h>
#include <libk/kio.h>
#include <libk/macros.h>
#include <libk/queue.h>
#include <libk/vga_kgfx.h>
#include <mem/kheap.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <multiboot.h>
#include <pic.h>
#include <stddef.h>
#include <stdint.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>
#include <x86_64.h>

extern void kernel_main(void);
extern void change_stacks(void);

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

void kernel_secondary_start(void);

void create_local_proccess(void) {
  PCB_t *pcb = create_process();
  TCB_t *tcb = create_thread(pcb, NULL);

  pcb->state = PROCESS_RUNNING;
  tcb->state = THREAD_RUNNING;

#define FS_MSR 0xC0000100

  wrmsr(FS_MSR, (size_t)tcb);
}

void kernel_start(uint8_t *multiboot) {

  //  test_print(multiboot);

  handle_init_array();
  init_multiboot(multiboot);

  init_memory_manager();
  kgfx_init();
  kgfx_clear();

  kio_printf("Initialized Memory Manager\n");

  init_global_brk();
  kio_printf("Initialized Global Heap (brk)\n");

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
  // extern char ex0nd_kernel[];
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

  // Set up default kernel region. during smp startup each core should make
  // their kernel region
  create_local_proccess();

  change_stacks();
}

void kernel_secondary_start(void) {
  init_heap();
  kio_printf("Initialized the heap (kernel malloc)\n");

  init_cls();
  kio_printf("Initialized CLS (Core Local Storage)\n");

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

  init_x86_64_hal();
  kio_printf("Initialized HAL\n");

  start_cores();

  kernel_main();
}
