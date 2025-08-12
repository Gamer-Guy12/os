#include <threading.h>

void idle(void) {
  while (1) {
    run_next_thread();
  }
}

