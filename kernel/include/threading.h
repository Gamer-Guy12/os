#ifndef THREADING_H
#define THREADING_H

#include <decls.h>
#include <stddef.h>

void run_next_thread(void);

void NORETURN kill_cur_thread(void);

#endif

