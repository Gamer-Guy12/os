#include <stdatomic.h>
#include <threading/threading.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <libk/spinlock.h>

PCB_t* process_list = NULL;
static spinlock_t lock = ATOMIC_FLAG_INIT;

void store_process(PCB_t* pcb) {
  spinlock_acquire(&lock);

  if (process_list == NULL) {
    process_list = pcb;
  } else {
    pcb->next = process_list;
    pcb->prev = NULL;
    process_list->prev = pcb;
    process_list = pcb;
  }

  spinlock_release(&lock);
}

void remove_process(PCB_t* pcb) {
  spinlock_acquire(&lock);

  if (pcb->prev)
    pcb->prev->next = pcb->next;
  if (pcb->next)
    pcb->next->prev = pcb->prev;

  spinlock_release(&lock);
}

PCB_t* get_proc_list(void) {
  return process_list;
}

