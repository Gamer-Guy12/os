#ifndef _x86_64_ACPI_H_
#define _x86_64_ACPI_H_

#include <stdint.h>

#define MADT "APIC"
#define DSDT "DSDT"
#define FADT "FACP"
#define SRAT "SRAT"
#define SSDT "SSDT"
#define XSDT "XSDT"

struct rsdp {
  char signature[8];
  uint8_t checksum;
  char oemid[6];
  uint8_t revision;
  /// DO NOT USE
  uint32_t rsdt;
  // 20 bytes before this

  uint32_t length;
  uint64_t xsdt;
  uint8_t extended_checksum;
  uint8_t reserved[3];
} __attribute__((packed));

struct sdt_header {
  char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char oemid[6];
  char oem_table_id[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
} __attribute__((packed));

struct xsdt {
  struct sdt_header header;
  uint64_t sdts[];
} __attribute__((packed));

#define MADT_LAPIC 0
#define MADT_IOAPIC 1
#define MADT_IOAPIC_OVERRIDE 2
#define MADT_IOAPIC_NM_SOURCES 3
#define MADT_LAPIC_NM_INTERRUPTS 4
#define MADT_LAPC_ADDR_OVERRIDE 5
#define MADT_x2APIC 9

struct madt_entry {
  uint8_t type;
  uint8_t length;
} __attribute__((packed));

enum madt_lapic_flags {
  MADT_PROC_ENABLED = 1,
  MADT_ONLINE_CAPABLE = (1 << 1)
};

struct madt_lapic {
  uint8_t apic_processor_id;
  uint8_t apic_id;
  uint32_t flags;
}__attribute__((packed));

struct madt_ioapic {
  uint8_t ioapic_id;
  uint8_t reserved;
  uint32_t ioapic_addr;
  uint32_t gsi_base;
}__attribute__((packed));

struct madt_ioapic_override {
  uint8_t bus_source;
  uint8_t irq_source;
  uint32_t gsi_interrupt;
  uint16_t flags;
}__attribute__((packed));

// More tables added later

struct madt {
  struct sdt_header header;
  uint32_t lapic_address;
  uint32_t flags;
} __attribute__((packed));

void init_rsdp(void);
void init_xsdt(void);
struct xsdt *get_xsdt(void);
void init_acpi(void);
void *get_acpi_table(char *signature);

#endif
