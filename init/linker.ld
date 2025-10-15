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
}
