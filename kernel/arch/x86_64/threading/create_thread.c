#include <mem/memory.h>
#include <stdatomic.h>
#include <stddef.h>
#include <threading/tcb.h>
#include <threading/threading.h>

static size_t tid_cur = 0;
static spinlock_t tid_lock = ATOMIC_FLAG_INIT;

size_t create_id_thread(void) {
  spinlock_acquire(&tid_lock);
  size_t ret = tid_cur;
  tid_cur++;
  spinlock_release(&tid_lock);

  return ret;
}

TCB_t *create_thread(PCB_t *process, void (*entry_point)(void)) {
  TCB_t *tcb = gmalloc(sizeof(TCB_t));

  tcb->rip0 = (size_t)entry_point;

  spinlock_acquire(&tid_lock);
  tcb->tid = tid_cur;
  tid_cur++;
  spinlock_release(&tid_lock);

  tcb->pcb = process;

  tcb->rsp0 =
      (size_t)create_new_kernel_stack(process->kernel_region, &tcb->stack_num);

  tcb->state = THREAD_STARTING;
  tcb->rip0 = (size_t)entry_point;

  spinlock_acquire(&process->pcb_lock);
  tcb->next = process->tcbs;
  tcb->prev = NULL;
  if (process->tcbs)
    process->tcbs->prev = tcb;
  process->tcbs = tcb;
  spinlock_release(&process->pcb_lock);

  tcb->registers = gmalloc(sizeof(registers_t));

  queue_thread(tcb);

  return tcb;
}
