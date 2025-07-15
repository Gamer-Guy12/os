#include <mem/kheap.h>
#include <threading/tcb.h>
#include <threading/pcb.h>
#include <asm.h>

heap_info_t *get_heap_info(void) {
  TCB_t *tcb = (TCB_t *)rdmsr(0xC0000100); 

  return &tcb->pcb->heap_info;
}
