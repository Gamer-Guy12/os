ENTRY(kinit)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS {
  . = 0xFFFFFFFF80000000;

  _start_kernel = .;

  .init.text ALIGN(4k) : {
    _start_init_text = .;

    *(.init.text)

    _end_init_text = .;
  }

  .init.rodata ALIGN(4k) : {
    _start_init_rodata = .;

    *(.init.rodata)

    _end_init_rodata = .;
  }

  .init.data ALIGN(4k) : {
    _start_init_data = .;

    *(.init.data)

    _end_init_data = .;
  }

  .init.bss ALIGN(4k) : {
    _start_init_bss = .;

    *(.init.bss)

    _end_init_bss = .;
  }

  .text ALIGN(4k) : {
    _start_text = .;
    
    *(.text)

    _end_text = .;
  }

  .rodata ALIGN(4k) : {
    _start_rodata = .;

    *(.rodata)

    _end_rodata = .;
  }

  .data ALIGN(4k) : {
    _start_data = .;

    *(.data)

    _end_data = .;
  }

  .bss ALIGN(4k) : {
    _start_bss = .;

    *(.bss)

    _end_bss = .;
  }

  _end_kernel = .;
}

