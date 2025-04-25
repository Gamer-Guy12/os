ENTRY(_start)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS {
	. = 0x100000;

    .text BLOCK(4k) : ALIGN(4k) {
        _text = .;
		*(.multiboot)
        *(.loading)
		
		. = 0x200000;

		*(.startup)
		*(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
	{
		_rodata = .;
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		_data = .;

		__init_array_start = .;
  		KEEP(*( .init_array ));
  		KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
  		__init_array_end = .;

		*(.data)
	}

	.eh_frame BLOCK(4K) : ALIGN(4K) {
		_ehframe = .;
		*(.eh_frame)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		_bss = .;
		*(.bss)

		*(COMMON)
	}

	end_kernel = .;
	. = 0x1000000;
	kernel_mem_end = .;

 	/* This is general purpose kernel memory, on x86_64 this is just used for the physical memory allocator and maybe later virtual memory allocator */
	kernel_gp = .;
	. = 0x2000000;
	kernel_gp_end = .;
}