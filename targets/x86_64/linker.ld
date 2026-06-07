ENTRY(kinit)
OUTPUT_FORMAT(elf64-x86-64)

PHDRS {
  limine_requests PT_LOAD;
  text PT_LOAD;
  rodata PT_LOAD;
  data PT_LOAD;
}

SECTIONS {
  /* + 0x1000000 is added to skip over the DMA region */
  . = 0xFFFFFFFF81000000;

  _start_kernel = .;

  .limine_requests ALIGN(4k) : {
    KEEP(*(.limine_requests_start))
    KEEP(*(.limine_requests))
    KEEP(*(.limine_requests_end))
  } :limine_requests

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
    
    . = ALIGN(4K);
    _start_cls = .;
    KEEP(*(.cls));
    _end_cls = .;

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

