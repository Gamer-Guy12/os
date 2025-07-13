
# SMP

- [x] read the MADT to find all the cores
- [x] create a trampoline to get to a point where kio_printf can be called
    - [x] Get into 32 bit mode
    - [x] Set up page tables
        - Create page tables in the os and store the address in some agreed upon place and then have all APs use the same one and then set up their own
        - Remeber to map the multiboot and the vga memory in
        - Before the AP starts up, map the kernel, map the recursive tables, and map the identity mapping, and the physical structures
        - After AP starts up map the multiboot and vga memory
    - [x] Get into 64 bit mode
    - [x] Jump to some 64 bit code
- [x] start all the cores up

# Threading

- [ ] Add to the handlers a part where it will store rsp and rip if the interrupt came from cpl != 0 and put them into the TCB

