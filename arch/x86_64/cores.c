#include "kernel/cores.h"
#include "limine.h"
#include "util.h"
#include "kernel/kprintf.h"
#include <stdint.h>

LIMINE_REQUEST static volatile struct limine_mp_request mp_request = {
    .id = LIMINE_MP_REQUEST, .flags = 0, .revision = 0};

static uint32_t bsp_id = 0;
static uint64_t cpu_count = 0;
static struct limine_mp_info **cpus;

static void entry(struct limine_mp_info *info) {
  kprintf("Hello World\n");

  while (1) {}
}

INIT void init_cores(void) {
  bsp_id = mp_request.response->bsp_lapic_id;
  cpu_count = mp_request.response->cpu_count;
  cpus = mp_request.response->cpus;

  for (uint64_t i = 0; i < cpu_count; i++) {
    if (cpus[i]->lapic_id == bsp_id) {
      continue;
    }

    __atomic_store_n(&cpus[i]->goto_address, entry, __ATOMIC_RELEASE);
  }
}

uint64_t get_core_count(void) { return cpu_count; }

uint32_t get_core_id(void) {
  uint32_t coreid = 0;
  __asm__ volatile("mov $1, %%eax; cpuid; shrl $24, %%ebx;" : "=b"(coreid));
  return coreid;
}

