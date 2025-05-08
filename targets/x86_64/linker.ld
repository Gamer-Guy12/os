ENTRY(_start)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS {
	. = 0x100000;

	.loading ALIGN(4K) :
	{
		_loading = .;

		*(.multiboot)
        *(.loading)

		*(.loading_rodata)
		*(.loading_data)
	}

	/* All these page tables will be replaced later on */
	.page_tables (NOLOAD) : ALIGN(4K)
	{
		_page_tables = .;
		_l4_page = .;
		. = _page_tables + 4K;
		_l3_page = .;
		. = _l3_page + 8K;
		_l2_page = .;
		. = _l2_page + 16k;
		_l1_page = .;
		. = _l1_page + 8M;
		_higher_l2_page = .;
		/* There is a maximum of 2 gb */
		. = _higher_l2_page + 8k;
		_higher_l1_page = .;
		. = _higher_l1_page + 4M;
	}

	.bootstrap_stack (NOLOAD) : ALIGN(4k) {
		_bs_stack_bottom = .;
		. = _bs_stack_bottom + 16K;
		_bs_stack_top = .;
	}

	. += 0xFFFFFFFF80000000;

	start_kernel = .;

    .text ALIGN(4k) : AT(ADDR(.text) - 0xFFFFFFFF80000000)
	{
		
		. += 0x200000;
        _text = .;

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

	. = ALIGN(4k);
	end_kernel = .;
	
	kernel_gp = .;

 	/* This is general purpose kernel memory, on x86_64 this is just used for the physical memory allocator and maybe later virtual memory allocator */
	/* This space should only be used for the initial memory managment bootstrap before something new is created */
	/* This is just here to clear some space and provide consistent addressing */
	/* Can and should be allocated over in physical memory */
	. = kernel_gp + 0x1000000;
	. = ALIGN(2M);
	kernel_gp_end = .;
}