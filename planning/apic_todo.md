# TODO

1. [x] Write the base msr or smth
2. [x] Reserve the apic in physical memory
3. [x] Map the apic into virtual memory (prolly in that 258 pdpt)
    - Imma just map it using that phys thingy or smth (257 pdpt) (aka no mapping just using)
4. [x] Set the spurious interrupt vector
5. [ ] Create all the functions for the hal irq
