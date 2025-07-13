#include <asm.h>
#include <threading/pcb.h>
#include <mem/memory.h>
#include <threading/tcb.h>

#define FS_MSR 0xC0000100

vmm_kernel_region_t* get_kernel_region(void) {
  TCB_t* tcb = (TCB_t*) rdmsr(FS_MSR);

  return tcb->pcb->kernel_region;
}

