#include <threading.h>
#include <asm.h>
#include <threading/threading.h>

PCB_t* get_cur_pcb(void) {
  return TCB->pcb;
}

TCB_t* get_cur_tcb(void) {
  return TCB;
}

