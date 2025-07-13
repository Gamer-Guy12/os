#ifndef X86_64_PCB_H
#define X86_64_PCB_H

#include <mem/memory.h>
#include <stddef.h>

typedef struct TCB_struct TCB_t;

typedef enum {
  PROCESS_RUNNING = 1,
  PROCESS_WAITING = 2,
  PROCESS_TERMINATED = 3
} PCB_state_t;

typedef struct PCB_struct {
  size_t pid;
  void* cr3;
  vmm_kernel_region_t* kernel_region;
  vmm_region_t* user_region;
  PCB_state_t state;
  struct PCB_struct* next;
  TCB_t* tcbs;
} PCB_t;

#endif

