#include "lib/atomic.h"

int atomic_add(atomic_t *atomic, int num) {
  return __atomic_add_fetch(&atomic->num, num, __ATOMIC_RELEASE);
}

int atomic_sub(atomic_t* atomic, int num) {
  return __atomic_sub_fetch(&atomic->num, num, __ATOMIC_RELEASE);
}

int atomic_load(atomic_t* atomic) {
  return __atomic_load_n(&atomic->num, __ATOMIC_RELEASE);
}

void atomic_store(atomic_t* atomic, int num) {
  __atomic_store_n(&atomic->num, num, __ATOMIC_RELEASE);
}

