#ifndef X86_64_THREADING_H
#define X86_64_THREADING_H

#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>

/// This swaps kernel threads, it is the job of the kernel thread to go (back) to user mode
/// This also requires that the tcb must be in a running state
void swap_threads(TCB_t* tcb);

PCB_t* create_process(void);
TCB_t* create_thread(PCB_t* process, void (*entry_point)(void));
size_t create_id_thread(void);

void delete_process(PCB_t* pcb);
/// This thread cannot be in the process queue
void delete_thread(TCB_t* tcb);

/// @param pop should this also pop it off the front
PCB_t* get_first_process(bool pop);
void append_process(PCB_t* process);

void queue_thread(TCB_t* tcb);
TCB_t* pop_thread(void);

void execute_next_thread(void);
void kill_thread(void);

#endif

