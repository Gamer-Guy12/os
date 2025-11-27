#ifndef _KERNEL_CORES_H_
#define _KERNEL_CORES_H_

#include <stddef.h>
#include <stdint.h>

void init_cores(void);
void core_entry(void);

uint64_t get_core_count(void);
uint32_t get_core_id(void);

// This variable will be initialized with the size of the variable but after
// initialization it will contain the offset from the cls base
#define CLS(type, name)                                                        \
  __attribute__((section(".cls"))) size_t name = sizeof(type)

// Must be called from every core
void init_cls(void);

#ifdef _x86_64_
#include "asm.h"
#include "x86_64.h"

#define GET_CLS(name) ((void *)(rdmsr(GS_BASE_MSR) + name))
#define CHECK_CLS(name) ((void *)rdmsr(GS_BASE_MSR) != NULL)
#else
#error "Cannot read CLS"
#endif

#endif
