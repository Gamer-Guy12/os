#include "lib/atomic.h"

int atomic_add(atomic_t *atomic, int num) {
  return __atomic_add_fetch(&atomic->num, num, __ATOMIC_ACQ_REL);
}

int atomic_sub(atomic_t* atomic, int num) {
  return __atomic_sub_fetch(&atomic->num, num, __ATOMIC_ACQ_REL);
}

int atomic_load(atomic_t* atomic) {
  return __atomic_load_n(&atomic->num, __ATOMIC_ACQUIRE);
}

void atomic_store(atomic_t* atomic, int num) {
  __atomic_store_n(&atomic->num, num, __ATOMIC_RELEASE);
}

bool atomic_cas(atomic_t* atomic, int old_val, int new_val) {
  return __atomic_compare_exchange_n(&atomic->num, &old_val, new_val, false, __ATOMIC_RELEASE, __ATOMIC_ACQUIRE);
}

