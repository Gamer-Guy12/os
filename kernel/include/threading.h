#ifndef THREADING_H
#define THREADING_H

#include <decls.h>
#include <stddef.h>

/// In MS
#define QUANTUM_LENGTH 10

void run_next_thread(void);

void NORETURN kill_cur_thread(void);

void idle(void);

#endif
