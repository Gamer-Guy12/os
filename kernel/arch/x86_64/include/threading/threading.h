#ifndef X86_64_THREADING_H
#define X86_64_THREADING_H

#include <interrupts.h>
#include <stddef.h>
#include <threading.h>
#include <threading/pcb.h>
#include <threading/tcb.h>

/// Loads a new thread
/// Returns the old thread
TCB_t *switch_threads(TCB_t *thread);
/// If a thread has state started then instead u want to start it
/// Returns the old thread
TCB_t *start_thread(TCB_t *thread);

PCB_t *create_process(void);
TCB_t *create_thread(PCB_t *process, void (*entry_point)(void));

void delete_process(PCB_t *pcb);
/// Thread is expected to not be in the queue when deleted
void delete_thread(TCB_t *tcb);

/// Create process calls this
void store_process(PCB_t *pcb);
/// Delete process calls this
void remove_process(PCB_t *pcb);
PCB_t *get_proc_list(void);
void clear_processes(void);

void queue_thread(TCB_t *tcb, thread_priority_t priority);
TCB_t *pop_thread(void);

void init_threading(void);

/// This is the function that new threads should go to, it will set up the
/// thread and then ret to it
void begin_thread(TCB_t* old);

#endif
