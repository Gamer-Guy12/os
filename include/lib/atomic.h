#ifndef _LIB_ATOMIC_H_
#define _LIB_ATOMIC_H_

typedef struct {
  int num;
} atomic_t;

int atomic_add(atomic_t *atomic, int num);
int atomic_sub(atomic_t *atomic, int num);
int atomic_load(atomic_t *atomic);
void atomic_store(atomic_t *atomic, int num);

#endif
