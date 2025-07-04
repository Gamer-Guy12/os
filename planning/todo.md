
# SMP

- [x] read the MADT to find all the cores
- [-] create a trampoline to get to a point where kio_printf can be called
    - [x] Get into 32 bit mode
    - [-] Set up page tables
        - Create page tables in the os and store the address in some agreed upon place and then have all APs use the same one and then set up their own
        - Remeber to map the multiboot and the vga memory in
        - Before the AP starts up, map the kernel, map the recursive tables, and map the identity mapping, and the physical structures
        - After AP starts up map the multiboot and vga memory
    - [ ] Get into 64 bit mode
    - [ ] Jump to some 64 bit code
- [ ] start all the cores up
