ENTRY(kinit)
OUTPUT_FORMAT(elf64-x86-64)

PHDRS {
  limine_requests PT_LOAD;
  init_text PT_LOAD;
  init_data PT_LOAD;
  text PT_LOAD;
  rodata PT_LOAD;
  data PT_LOAD;
}

SECTIONS {
  . = 0xFFFFFFFF80000000;

  _start_kernel = .;

  .limine_requests ALIGN(4k) : {
    KEEP(*(.limine_requests_start))
    KEEP(*(.limine_requests))
    KEEP(*(.limine_requests_end))
  } :limine_requests

  .init.text ALIGN(4k) : {
    _start_init_text = .;

    *(.init.text)

    _end_init_text = .;
  } :init_text

  .init.rodata ALIGN(4k) : {
    _start_init_rodata = .;

    *(.init.rodata)

    _end_init_rodata = .;
  } :init_data

  .init.data ALIGN(4k) : {
    _start_init_data = .;

    *(.init.data)

    . = ALIGN(8);
    _start_cls = .;
    *(.cls)
    _end_cls = .;

    _end_init_data = .;
  } :init_data

  .init.bss ALIGN(4k) : {
    _start_init_bss = .;

    *(.init.bss)

    _end_init_bss = .;
  } :init_data

  .text ALIGN(4k) : {
    _start_text = .;
    
    *(.text)

    _end_text = .;
  } :text
  . = ALIGN(4k);

  .rodata ALIGN(4k) : {
    _start_rodata = .;

    *(.rodata)

    _end_rodata = .;
  } :rodata

  .data ALIGN(4k) : {
    _start_data = .;

    *(.data)

    _end_data = .;
  } :data

  .bss ALIGN(4k) : {
    _start_bss = .;

    *(.bss)

    _end_bss = .;
  } :data

  _end_kernel = .;

  /DISCARD/ : {
    *(.eh_frame*)
    *(.note .note.*)
  }
}

