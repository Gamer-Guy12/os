#ifndef _LIB_ATOMIC_H_
#define _LIB_ATOMIC_H_

#include <stdbool.h>

typedef struct {
  int num;
} atomic_t;

int atomic_add(atomic_t *atomic, int num);
int atomic_sub(atomic_t *atomic, int num);
int atomic_load(atomic_t *atomic);
void atomic_store(atomic_t *atomic, int num);
bool atomic_cas(atomic_t *atomic, int old_val, int new_val);

__attribute__((unused)) static const atomic_t ATOMIC_ZERO = {0};

#endif
