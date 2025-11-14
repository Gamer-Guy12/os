#ifndef _KERNEL_CLS_H_
#define _KERNEL_CLS_H_

#include "util.h"
#include <stddef.h>
#include <stdint.h>

#define CLS(type, name)                                                        \
  __attribute__((section(".cls"))) size_t name = sizeof(type)

#ifdef _x86_64_
#include "asm.h"

#define READ_CLS(name) (void *)(rdmsr(0xC0000101) + name)
#else
#error "Cannot generate READ_CLS macro for this architecture"
#endif

SASSERT(sizeof(size_t) == sizeof(uintptr_t),
        "A size quantifier cannot hold a pointer in this architecture."
        "Please update cls code to fix this.");

void init_cls(void);
void init_core_cls(void);

#endif
