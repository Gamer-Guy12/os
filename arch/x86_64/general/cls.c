#include "kernel/cls.h"
#include "kernel/cores.h"
#include <stdint.h>

extern void *cls_tables;
extern size_t cls_stride;

void init_core_cls(void) {
  uintptr_t this_cls_table = (uintptr_t)cls_tables + cls_stride * get_core_id();
  wrmsr(0xC0000101, this_cls_table);
}

