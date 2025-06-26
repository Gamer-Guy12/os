#ifndef X86_64_MULTIBOOT_H
#define X86_64_MULTIBOOT_H

#include <stdint.h>
#include <decls.h>

#define MLTBT_BASIC_MEM_INFO 4
#define MLTBT_BIOS_BOOT_DEV 5
#define MLTBT_BOOT_CMD 1
#define MLTBT_MODULES 3
#define MLTBT_ELF_SYMBOLS 9
#define MLTBT_MEM_MAP 6
#define MLTBT_BOOTLOADER_NAME 2
#define MLTBT_APM_TABLE 10
#define MLTBT_VBE_INFO 7
#define MLTBT_FRAMEBUFFER_INFO 8
#define MLTBT_EFI_32_STP 11
#define MLTBT_EFI_64_STP 12
#define MLTBT_SMBIOS_TABLES 13
#define MLTBT_RSDP 14
#define MLTBT_XSDP 15
#define MLTBT_NETWORK_INFO 16
#define MLTBT_EFI_MEM_MAP 17
#define MLTBT_EFI_SERVICES_NOT_TERM 18
#define MLTBT_EFI_32_IMAGE_HANDLE_PTR 19
#define MLTBT_EFI_64_IMAGE_HANDLE_PTR 20
#define MLTBT_IMAGE_LOAD_BASE_PHYS_ADDR 21

typedef struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} PACKED mltbt_framebuffer_palette_instance;

typedef struct {
  uint32_t palette_size;
} PACKED mltbt_framebuffer_type_0_t;

typedef struct {
  uint8_t framebuffer_red_field_pos;
  uint8_t framebuffer_red_mask_size;
  uint8_t framebuffer_green_field_pos;
  uint8_t framebuffer_green_mask_size;
  uint8_t framebuffer_blue_field_pos;
  uint8_t framebuffer_blue_mask_size;
} PACKED mltbt_framebuffer_type_1_t;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint64_t framebuffer_addr;
  uint32_t framebuffer_pitch;
  uint32_t framebuffer_width;
  uint32_t framebuffer_height;
  uint8_t framebuffer_bpp;
  uint8_t framebuffer_type;
  uint8_t reserved;
} PACKED mltbt_framebuffer_info_t;

void *multiboot_get_tag(uint32_t type);

#endif
