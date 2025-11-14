#include "kernel/cores.h"
#include "limine.h"
#include "util.h"
#include "asm.h"
#include <stddef.h>

LIMINE_REQUEST static volatile struct limine_mp_request mp_request = {
    .id = LIMINE_MP_REQUEST, .flags = 0, .revision = 0};

static struct limine_mp_response *mp_response = NULL;
static size_t cpu_count = 0;
static size_t bsp_lapic_id = 0;

void INIT setup_core_info(void) {
  mp_response = mp_request.response;

  cpu_count = mp_response->cpu_count;
  bsp_lapic_id = mp_response->bsp_lapic_id;
}

size_t get_core_count(void) {
  return cpu_count;
}

size_t get_core_id(void) {
  uint32_t ebx = 0;
  cpuid_all(1, NULL, &ebx, NULL, NULL);

  return ebx >> 24 & 0xFF;
}
