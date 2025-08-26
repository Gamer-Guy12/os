#ifndef X86_64_HPET_H
#define X86_64_HPET_H

#include <acpi/acpi.h>
#include <decls.h>
#include <libk/math.h>
#include <mem/pimemory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef const volatile struct {
  uint8_t addr_space_id;
  uint8_t register_bit_width;
  uint8_t register_bit_offset;
  uint8_t reserved;
  uint64_t addr;
} PACKED addr_struct_t;

typedef const volatile struct {
  sdt_header_t header;
  uint8_t hardware_revision;
  uint8_t comparator_count : 5;
  uint8_t counter_size : 1;
  uint8_t reserved : 1;
  uint8_t legacy_replacement : 1;
  uint16_t pci_vendor_id;
  addr_struct_t address;
  uint8_t hpet_number;
  uint16_t min_ticks;
  uint8_t page_protection;
} PACKED HPET_t;

typedef const volatile struct {
  uint8_t rev_id;
  /// Timer Count minus 1
  uint8_t timer_count : 5;
  uint8_t count_size_cap : 1;
  uint8_t reserved : 1;
  uint8_t legacy_replacement_cap : 1;
  uint16_t vendor_id;
  uint32_t clock_period;
} PACKED HPET_gen_caps_t;

typedef volatile struct {
  uint64_t enable_timer : 1;
  uint64_t legacy_mapping_enabled : 1;
  uint64_t reserved : 62;
} PACKED HPET_gen_config_t;

typedef volatile struct {
  union {
    struct {
      uint16_t reserved_0 : 1;
      /// 1 is level triggered
      uint16_t trigger_type : 1;
      uint16_t int_enable : 1;
      uint16_t periodic : 1;
      uint16_t supports_periodic : 1;
      uint16_t supports_64bit : 1;
      uint16_t direct_set_accumulator : 1;
      uint16_t reserved_1 : 1;
      uint16_t force32 : 1;
      uint16_t ioapic_route : 5;
      uint16_t fsb_int_mapping : 1;
      uint16_t supports_fsb : 1;
      uint16_t reserved_2;
      /// If bit X is set then this can be routed to irq X
      uint32_t ioapic_support_bit;
    };
    uint64_t value;
  };
} PACKED HPET_timer_config_caps_t;

#define HPET_GEN_CAPS 0x0
#define HPET_GEN_CONFIG 0x10
#define HPET_GEN_INT_STATUS 0x20
#define MAIN_COUNTER_VALUE 0xF0

#define HPET_TIMER_CONFIG_CAP(timer) (0x100 + 0x20 * timer)
#define HPET_TIMER_COMPARATOR_VAL(timer) (0x108 + 0x20 * timer)
#define HPET_TIMER_FSB_ROUTE(timer) (0x110 + 0x20 * timer)

#define HPET_ADDR INDICES_TO_ADDR(0, 0, 255ull, 258ull)

/// Returns the amount of apics there are
bool WUNUSED check_for_hpet(void);
size_t enable_hpet(void);
bool WUNUSED get_int_status(uint8_t timer);

uint128_t hpet_cycles_wait(uint64_t ms);
uint128_t hpet_get_progress(uint8_t hpet);
void hpet_interrupt_in_cycles(uint128_t cycles, uint8_t hpet);
void hpet_interrupt_in(uint64_t ms, uint8_t hpet);
uint8_t reserve_hpet(void);
void return_hpet(uint8_t hpet);
void bind_hpet_callback(void (*callback)(void), uint8_t hpet);

#define NO_HPET 32

#endif
