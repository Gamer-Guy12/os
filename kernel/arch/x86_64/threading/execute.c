#include <asm.h>
#include <cls.h>
#include <libk/kio.h>
#include <libk/queue.h>
#include <stdbool.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

#define FS_MSR 0xC0000100

void run_next_thread(void) {
  TCB_t *tcb = (TCB_t *)rdmsr(FS_MSR);
  // Increment the amount of quantums used
  tcb->rb_node.value++;
  // This means that if there are no threads this thread could be queued again
  if (tcb->state != THREAD_TERMINATED) {
    queue_thread(tcb, tcb->priority);
  }

  cls_t *cls = get_cls();
  TCB_t *next = pop_thread();

  queue_node_t *load_next = queue_dequeue(&cls->load_queue);

  while (load_next != NULL) {
    TCB_t *load = (TCB_t *)((size_t)load_next - offsetof(TCB_t, queue_node));
    queue_thread(load, load->priority);

    load_next = queue_dequeue(&cls->load_queue);
  }

  while (next == NULL) {
    queue_node_t *load_next = queue_dequeue(&cls->load_queue);

    while (load_next != NULL) {
      TCB_t *load = (TCB_t *)((size_t)load_next - offsetof(TCB_t, queue_node));
      queue_thread(load, load->priority);

      load_next = queue_dequeue(&cls->load_queue);
    }

    next = pop_thread();
  }

  if (tcb->state == THREAD_TERMINATED) {
    cls_t *cls = get_cls();
    queue_enqueue(&cls->dead_queue, &tcb->queue_node);
  }

   volatile int num = 1;
  size_t num2 = 0;
  while (num == 1) {
    num2++;
    if (num2 == 100000000) num = 0; 
  }

  // kio_printf("%x %x %x\n", tcb->rsp0, tcb->rip0, tcb->registers->ss);
  __sync_synchronize();
  __asm__ volatile ("mfence" ::: "memory");
  swap_threads(next);
}
