#ifndef X86_64_THREADING_H
#define X86_64_THREADING_H

#include <threading/pcb.h>
#include <threading/tcb.h>

/// This swaps kernel threads, it is the job of the kernel thread to go (back) to user mode
/// This also requires that the tcb must be in a running state
void swap_threads(TCB_t* tcb);

PCB_t* create_process(void);
TCB_t* create_thread(PCB_t* process, void (*entry_point)(void), bool queue);

void delete_process(PCB_t* pcb);
/// Thread is expected to not be in the queue when deleted
void delete_thread(TCB_t* tcb);

/// Create process calls this
void store_process(PCB_t* pcb);
/// Delete process calls this
void remove_process(PCB_t* pcb);
PCB_t* get_proc_list(void);
void clear_processes(void);

/// Create thread calls this
void queue_thread(TCB_t* tcb);
TCB_t* pop_thread(void);

void run_next_thread(void);

void kill_cur_thread(void);

#endif

