#ifndef THREADING_H
#define THREADING_H

#include <threading/tcb.h>
#include <decls.h>
#include <stddef.h>

/// In MS
#define QUANTUM_LENGTH 10

void run_next_thread(void);

void NORETURN kill_cur_thread(void);

PCB_t *WUNUSED create_process(void);
TCB_t *WUNUSED create_thread(PCB_t *process, void (*entry_point)(void));

void delete_process(PCB_t *pcb);
/// Thread is expected to not be in the queue when deleted
void delete_thread(TCB_t *tcb);

void schedule_thread(TCB_t *tcb, thread_priority_t priority);

void idle(void);

#endif
