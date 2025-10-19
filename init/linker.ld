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

  .limine_requests_start : {
    KEEP(*(.limine_requests_start))
  }

  .limine_requests : {
    KEEP(*(.limine_requests))
  }

  .limine_requests_end : {
    KEEP(*(.limine_requests_end))
  }
}
