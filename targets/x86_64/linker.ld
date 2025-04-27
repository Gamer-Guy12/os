ENTRY(_start)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS {
	. = 0x100000;

	.loading : ALIGN(4K) 
	{
		_loading = .;

		*(.multiboot)
        *(.loading)
	}

	. += 0xFFFFFFFF80000000;

    .text ALIGN(4k) : AT(ADDR(.text) - 0xFFFFFFFF80000000)
	{
        _text = .;
		
		. = 0x200000;

		*(.startup)
		*(.text)
    }

    .rodata ALIGN(4k) : AT(ADDR(.rodata) - 0xFFFFFFFF80000000)
	{
		_rodata = .;
		*(.rodata)
	}

	.data ALIGN(4k) : AT(ADDR(.data) - 0xFFFFFFFF80000000)
	{
		_data = .;

		__init_array_start = .;
  		KEEP(*( .init_array ));
  		KEEP(*(SORT_BY_INIT_PRIORITY( .init_array.* )));
  		__init_array_end = .;

		*(.data)
	}

	.eh_frame ALIGN(4k) : AT(ADDR(.eh_frame) - 0xFFFFFFFF80000000)
	{
		_ehframe = .;
		*(.eh_frame)
	}

	.bss ALIGN(4k) : AT(ADDR(.bss) - 0xFFFFFFFF80000000)
	{
		_bss = .;
		*(.bss)

		*(COMMON)
	}

	end_kernel = .;
	. = 0x1000000 + 0xFFFFFFFF80000000;
	kernel_mem_end = .;

 	/* This is general purpose kernel memory, on x86_64 this is just used for the physical memory allocator and maybe later virtual memory allocator */
	/* This space should only be used for the initial memory managment bootstrap before something new is created */
	/* This is just here to clear some space and provide consistent addressing */
	/* Can and should be allocated over in physical memory */
	kernel_gp = .;
	. = 0x2000000 + 0xFFFFFFFF80000000;
	kernel_gp_end = .;
}