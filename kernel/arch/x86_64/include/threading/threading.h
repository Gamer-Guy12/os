#ifndef X86_64_THREADING_H
#define X86_64_THREADING_H

#include <threading/tcb.h>

/// This swaps kernel threads, it is the job of the kernel thread to go (back) to user mode
/// This also requires that the tcb must be in a running state
void swap_threads(TCB_t* tcb);

#endif

