#include <asm.h>
#include <cls.h>
#include <libk/spinlock.h>
#include <mem/memory.h>
#include <stdatomic.h>
#include <stddef.h>

cls_t *cls_list = NULL;
size_t cls_count = 0;
spinlock_t cls_lock = ATOMIC_FLAG_INIT;

void init_cls(void) {
  cls_t *cls = gmalloc(sizeof(cls_t));

  spinlock_acquire(&cls_lock);
  cls->next = cls_list;
  cls_list = cls;
  cls_count++;
  spinlock_release(&cls_lock);

  cls->tcb_queue = NULL;

#define GS_BASE_MSR 0xC0000101

  wrmsr(GS_BASE_MSR, (size_t)cls);
}

cls_t *get_cls(void) {
  size_t value = rdmsr(GS_BASE_MSR);

  return (cls_t *)value;
}

cls_t *get_cls_list(void) { return cls_list; }

size_t get_cls_count(void) { return cls_count; }

