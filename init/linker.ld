SECTIONS {
  .init.text : {
    *(.text)
  }

  .init.rodata : {
    *(.rodata)
  }

  .init.data : {
    *(.data)
  }

  .init.bss : {
    *(.bss)
  }

  .limine_requests : {
    KEEP(*(.limine_requests_start))
    KEEP(*(.limine_requests))
    KEEP(*(.limine_requests_end))
  }
}
