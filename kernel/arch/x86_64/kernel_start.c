#include "threading/pcb.h"
#include "threading/tcb.h"
#include "threading/threading.h"
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
#include <libk/vga_kgfx.h>
#include <mem/kheap.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <multiboot.h>
#include <pic.h>
#include <stddef.h>
#include <stdint.h>
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
  vmm_kernel_region_t *region = gmalloc(sizeof(vmm_kernel_region_t));
  create_kernel_region(region);
  vmm_kernel_region_t **region_ptr = KERNEL_REGION_PTR_LOCATION;
  *region_ptr = region;

  change_stacks();
}

PCB_t *pcb;
TCB_t *tcb;

PCB_t *pcb2;
TCB_t *tcb2;

void *stack2;

void NORETURN to_thread(void) {
  kio_printf("Other Thread!\n");

  swap_threads(tcb);

  kio_printf("Im back\n");

  swap_threads(tcb);

  while (1) {
  }
}

void test_threading(void) {
  pcb = gmalloc(sizeof(PCB_t));
  tcb = gmalloc(sizeof(TCB_t));
  registers_t *registers = gmalloc(sizeof(registers_t));

  pcb->tcbs = tcb;
  tcb->pcb = pcb;

  pcb->cr3 = (void *)virt_to_phys(PML4_ADDR);
  pcb->kernel_region = *KERNEL_REGION_PTR_LOCATION;
  pcb->state = PROCESS_RUNNING;
  pcb->pid = 0;

  tcb->state = THREAD_RUNNING;
  tcb->tid = 0;
  tcb->stack_num = 0;
  tcb->registers = registers;

  pcb2 = gmalloc(sizeof(PCB_t));
  tcb2 = gmalloc(sizeof(TCB_t));
  registers_t *registers2 = gmalloc(sizeof(registers_t));
  stack2 = phys_alloc();

  pcb2->tcbs = tcb2;
  tcb2->pcb = pcb2;

  pcb2->cr3 = (void *)virt_to_phys(PML4_ADDR);
  pcb2->kernel_region = *KERNEL_REGION_PTR_LOCATION;
  pcb2->state = PROCESS_RUNNING;
  pcb2->pid = 1;

  tcb2->state = THREAD_RUNNING;
  tcb2->tid = 1;
  tcb2->stack_num = 0;
  tcb2->rip0 = (size_t)to_thread;
  tcb2->registers = registers2;
  tcb2->rsp0 = (size_t)stack2 + IDENTITY_MAPPED_ADDR + PAGE_SIZE - 8;
  registers2->cs = KERNEL_CODE_SELECTOR;
  registers2->ds = KERNEL_DATA_SELECTOR;
  registers2->es = KERNEL_DATA_SELECTOR;
  registers2->ss = KERNEL_DATA_SELECTOR;

  // Load the TCB into fs
  wrmsr(0xC0000100, (size_t)tcb);

  swap_threads(tcb2);

  kio_printf("Hi\n");

  swap_threads(tcb2);

  gfree(pcb2);
  gfree(tcb2->registers);
  gfree(tcb2);
  phys_free(stack2);
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

  test_threading();

  kernel_main();
}
