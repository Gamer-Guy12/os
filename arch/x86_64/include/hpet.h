#ifndef _x86_64_HPET_H_
#define _x86_64_HPET_H_

#include "acpi.h"
#include <stdint.h>

#define HPET_GEN_CAPS 0x0
#define HPET_GEN_CONF 0x10
#define HPET_GEN_INT_STATUS 0x20
#define HPET_MAIN_COUNTER 0xF0
#define HPET_TIMER_CONF_CAPS(n) (0x100 + 0x20 * (n))
#define HPET_TIMER_COMPARATOR_VAL(n) (0x108 + 0x20 * (n))
#define HPET_TIMER_FSB_ROUTE(n) (0x110 + 0x20 * (n))

struct hpet_table {
  struct sdt_header header;
  uint8_t hardware_revid;
  uint8_t comparator_count : 5;
  uint8_t counter_size : 1;
  uint8_t reserved_1 : 1;
  uint8_t legacy_replacement : 1;
  uint16_t pci_vendor_id;
  uint8_t address_space_id;
  uint8_t register_bit_width;
  uint8_t register_bit_offset;
  uint8_t reserved_2;
  uint64_t address;
  uint8_t hpet_number;
  uint16_t minimum_tick;
  uint8_t page_protection;
} __attribute__((packed));

void init_hpet(void);

#endif
