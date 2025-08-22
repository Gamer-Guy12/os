#ifndef THREADING_H
#define THREADING_H

#include <decls.h>
#include <stddef.h>
#include <stdint.h>

typedef enum { TP_IDLE, TP_NORMAL, TP_HIGH, TP_IO } thread_priority_t;

typedef struct semaphore_struct semaphore_t;
typedef struct TCB_struct TCB_t;
typedef struct PCB_struct PCB_t;

void semaphore_create(semaphore_t* semaphore, int64_t max);
/// Decrement value
void semaphore_wait(semaphore_t *semaphore);
/// Increment value
void semaphore_signal(semaphore_t* semaphore);

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

void sleep_for(uint32_t ms);

#endif
