#ifndef X86_64_THREADING_H
#define X86_64_THREADING_H

#include <threading/pcb.h>
#include <threading/tcb.h>

/// This swaps kernel threads, it is the job of the kernel thread to go (back) to user mode
/// This also requires that the tcb must be in a running state
void swap_threads(TCB_t* tcb);

PCB_t* create_process(void);
TCB_t* create_thread(PCB_t* process, void *entry_point);

void delete_thread(TCB_t* tcb);
void delete_process(PCB_t* pcb);

#endif

