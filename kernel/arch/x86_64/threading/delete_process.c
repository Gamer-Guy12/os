#include <asm.h>
#include <libk/err.h>
#include <libk/sys.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <threading/pcb.h>
#include <threading/threading.h>

#define FS_MSR 0xC0000100

// Can't be deleted by the current process
void delete_process(PCB_t *pcb) {
  PCB_t* cur_pcb = ((TCB_t*)rdmsr(FS_MSR))->pcb;
  if (pcb == cur_pcb) {
    sys_panic(SELF_TERM_ERR | PROCESS_SELF_TERM);
  }

  TCB_t *tcb = pcb->tcbs;

  while (tcb != NULL) {
    TCB_t *next = tcb->next;
    delete_thread(tcb);
    tcb = next;
  }

  delete_addr_space(pcb);

  remove_process(pcb);

  gfree(pcb);
}

