#include <asm.h>
#include <cls.h>
#include <mem/memory.h>
#include <stddef.h>

void init_cls(void) {
  cls_t *cls = kmalloc(sizeof(cls_t), 0);

#define GS_BASE_MSR 0xC0000101

  wrmsr(GS_BASE_MSR, (size_t)cls);
}

cls_t *get_cls(void) {
  size_t value = rdmsr(GS_BASE_MSR);

  return (cls_t *)value;
}
