#include "kernel/cores.h"
#include "asm.h"
#include "kernel/gheap.h"
#include "kernel/mem.h"
#include "lib/string.h"
#include "limine.h"
#include "util.h"
#include "x86_64.h"
#include <stddef.h>
#include <stdint.h>

LIMINE_REQUEST static volatile struct limine_mp_request mp_request = {
    .id = LIMINE_MP_REQUEST, .flags = 0, .revision = 0};

static uint32_t bsp_id = 0;
static uint64_t cpu_count = 0;
static struct limine_mp_info **cpus;

static struct {
  uintptr_t cr3;
} startup_info;

static void entry(struct limine_mp_info *info) {
  __asm__ volatile("mov %0, %%cr3" ::"r"(startup_info.cr3) : "memory");

  kinit();

  while (1) {
  }
}

INIT void init_cores(void) {
  bsp_id = mp_request.response->bsp_lapic_id;
  cpu_count = mp_request.response->cpu_count;
  cpus = mp_request.response->cpus;

  for (uint64_t i = 0; i < cpu_count; i++) {
    if (cpus[i]->lapic_id == bsp_id) {
      continue;
    }

    uintptr_t cr3 = 0;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    startup_info.cr3 = cr3;

    __atomic_store_n(&cpus[i]->goto_address, entry, __ATOMIC_RELEASE);
  }
}

uint64_t get_core_count(void) { return cpu_count; }

uint32_t get_core_id(void) {
  uint32_t coreid = 0;
  __asm__ volatile("mov $1, %%eax; cpuid; shrl $24, %%ebx;" : "=b"(coreid));
  return coreid;
}

extern char _start_cls[];
extern char _end_cls[];

static INIT_DATA size_t full_size = 0;
static INIT_DATA spinlock_t calculate = SPINLOCK_ZERO(cls_calc);

void init_cls(void) {
  if (spinlock_attempt(&calculate)) {
    uintptr_t start_cls = (uintptr_t)_start_cls;
    uintptr_t end_cls = (uintptr_t)_end_cls;
    size_t len = end_cls - start_cls;
    size_t count = len / sizeof(size_t);

    size_t cur_size = 0;

    for (size_t i = 0; i < count; i++) {
      size_t *entry = (void *)(start_cls + i * sizeof(size_t));

      cur_size += *entry;
      *entry = cur_size - *entry;
    }

    full_size = cur_size;
  }

  if (full_size == 0) {
    return;
  }

  void *data = gmalloc(full_size, ZONE_ANY);
  memset(data, 0, full_size);
  WRMSR(GS_BASE_MSR, data);
}

bool is_bsp(void) {
  if (mp_request.response->bsp_lapic_id == get_core_id()) {
    return true;
  }

  return false;
}
