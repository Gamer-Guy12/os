#include <libk/spinlock.h>
#include <mem/memory.h>
#include <mem/vimemory.h>
#include <stdatomic.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/threading.h>

// There are 18 trillion ids so probably will never wrap
// if it does then by then the process at pid 0 will probably be dead
static size_t cur_pid = 0;
static spinlock_t pid_lock = ATOMIC_FLAG_INIT;

PCB_t *create_process(void) {
  PCB_t *pcb = gmalloc(sizeof(PCB_t));

  create_new_addr_space(pcb);

  pcb->tcbs = NULL;
  pcb->state = PROCESS_STARTING;
  
  spinlock_acquire(&pid_lock);

  pcb->pid = cur_pid;
  cur_pid++;

  spinlock_release(&pid_lock);

  pcb->tcbs = NULL;

  return pcb;
}
