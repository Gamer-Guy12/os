#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>

typedef struct {
  char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t creator_id;
  uint32_t creator_revision;
} __attribute__((packed)) SDT_header_t;

typedef struct {
  SDT_header_t header;
  SDT_header_t *pointer[];
} __attribute__((packed)) XSDT_t;

typedef struct {
  char signature[8];
  uint8_t checksum;
  char OEMID[6];
  uint8_t revision;
  uint32_t rsdt_addr;

  uint32_t length;
  XSDT_t *xsdt_addr;
  uint8_t extended_checksum;
  uint8_t reserved[3];
} __attribute__((packed)) XSDP_t;

typedef struct {
  uint8_t type;
  uint8_t length;
} __attribute__((packed)) MADT_entry_t;

typedef struct {
  SDT_header_t header;
  uint32_t lapic_addr;
  uint32_t flags;
  uint8_t *entries;
} __attribute__((packed)) MADT_t;

XSDP_t *find_xsdp(uint8_t *multiboot);
XSDT_t *find_xsdt(XSDP_t *xsdp);
MADT_t *find_madt(XSDT_t *xsdt);

#endif
