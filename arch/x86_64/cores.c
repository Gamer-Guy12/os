#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "lib/atomic.h"
#include "lib/string.h"
#include "limine.h"
#include "util.h"
#include "x86_64.h"
#include <stddef.h>
#include <stdint.h>

LIMINE_REQUEST static volatile struct limine_mp_request mp_request = {
    .id = LIMINE_MP_REQUEST_ID,
    .flags = LIMINE_MP_REQUEST_X86_64_X2APIC,
    .revision = 0};

static uint32_t bsp_id = 0;
static uint64_t cpu_count = 0;
static struct limine_mp_info **cpus;

void check_apic(void) {
  if (!(mp_request.response->flags & LIMINE_MP_RESPONSE_X86_64_X2APIC)) {
    kprintf("No x2APIC onboard on core %u\n", get_core_id());
    panic();
  }
}

static struct {
  uintptr_t cr3;
} startup_info;

static void entry(struct limine_mp_info *info) {
  __asm__ volatile("mov %0, %%cr3" ::"r"(startup_info.cr3) : "memory");

  kinit();

  while (1) {
  }
}

void init_cores(void) {
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

bool is_bsp(void) {
  if (mp_request.response->bsp_lapic_id == get_core_id()) {
    return true;
  }

  return false;
}

uint32_t get_bsp_id(void) {
  return bsp_id;
}

extern char _start_cls[];
extern char _end_cls[];
static atomic_t init_state;

// Calculates sizes and offsets for each field
static void calculate_size(void) {
  if (!atomic_cas(&init_state, 0, 1)) {
    while (atomic_load(&init_state) != 2)
      ;
    return;
  }

  size_t *entries = (void *)_start_cls;
  size_t count = ((uintptr_t)_end_cls - (uintptr_t)_start_cls) / sizeof(size_t);

  size_t cur_size = 0;
  for (size_t i = 0; i < count; i++) {
    size_t entry_size = entries[i];
    entries[i] = cur_size;
    cur_size += entry_size;
  }

  atomic_store(&init_state, 2);
}

void init_cls(void) {
  calculate_size();

  size_t size = (uintptr_t)_end_cls - (uintptr_t)_start_cls;
  int order = -1;
  for (int i = 0; i < MAX_ORDER; i++) {
    size_t calculated_size = PAGE_SIZE * (1 << i);
    if (calculated_size >= size) {
      order = i;
      break;
    }
  }

  if (order == -1) {
    _kprintf("CLS to big!\n");
    panic();
  }

  void *ptr = _alloc_pages(order, ZONE_ANY);
  memset(ptr, 0, size);
  WRMSR(GS_BASE_MSR, ptr);
}
