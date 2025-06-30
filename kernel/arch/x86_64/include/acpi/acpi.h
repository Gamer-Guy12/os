#ifndef X86_64_ACPI_H
#define X86_64_ACPI_H

#include <decls.h>
#include <stdint.h>

typedef struct {
  char signature[8];
  uint8_t checksum;
  char OEMID[6];
  uint8_t revision;
  /// Don't use this
  uint32_t rsdt_addr;

  uint32_t length;
  uint64_t xsdt_addr;
  uint8_t extended_checksum;
  uint8_t reserved[3];
} PACKED XSDP_t;

typedef struct {
  char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char OEMID[6];
  char OEM_table_ID[8];
  uint32_t OEM_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
} PACKED sdt_header_t;

typedef struct {
  sdt_header_t sdt_header;
  uint64_t sdt_ptrs[];
} PACKED XSDT_t;

void *acpi_get_struct(char signature[4]);
XSDP_t *get_xsdp(void);
XSDT_t *get_xsdt(void);

#endif
