/// CLS stands for core local storage

#ifndef X86_64_CLS
#define X86_64_CLS

#include <decls.h>
#include <gdt.h>
#include <hal/kbd.h>
#include <interrupts.h>
#include <irq.h>
#include <libk/list.h>
#include <libk/queue.h>
#include <libk/rbtree.h>
#include <stddef.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

typedef struct {
  /// IMPORTANT: This should only be used to save the current tcb when it is
  /// necessary not for common use
  /// user fs is saved when in kernel space which makes it even more important
  /// not to mess with this
  TCB_t *cur_tcb;
  ALIGN(0x10) gdt_descriptor_t gdt[DESCRIPTOR_COUNT];
  tss_t *tss;
  void *true_addr;
  thread_queue_t thread_queue;
  list_node_t node;
  size_t feature_flags;
} cls_t;

void init_cls(size_t feature_flags);
cls_t *WUNUSED get_cls(void);
list_t *WUNUSED get_cls_list(void);
cls_t *WUNUSED get_cls_at(size_t index);
size_t WUNUSED get_core_count(void);

#endif
