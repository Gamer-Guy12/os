ENTRY(_start)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS {
	. = 0x100000;

    .text BLOCK(4k) : ALIGN(4k) {
        *(.multiboot)
        *(.loading)
		
		. = 0x200000;

		*(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(.bss)
	}
}