# OS

Makefile Usage:

Build Kernel:
```bash
make build
```
or
```bash
make debug
```
Build Kernel Image:
```bash
make image
```

Requirements:
- GCC x86_64-elf Cross Compiler (15.2.0)
- Binutils x86_64-elf Tools (2.45)
- Limine v10.2.0

## Function Prefixes
- "_" - Earlier version of function that doesn't require as much/Old function only used in init
- "__" - Internal function

# Memory Management

Allocate a page:
__alloc_page
__free_page

Takes in a zone

This one either returns null or allocates a page (no blocking other than spinlock)

Get a page with properties (also returns virtual instead of physical address):
alloc_page
free_page

Takes in a list of types 
- DMA (No Blocking, Zone DMA)
- Kernel (Blocking, Zone Low or Zone High)
- Critical (No Blocking, Zone Low or Zone High)
- User (Blocking, Zone Low or Zone High, Movable)
- etc.

these flags map to
- Zone
- Manner (how the page should be gotten e.g. No blocking)
- Page Settings (Movable, User)

Dynamic Allocation:
gmalloc
gfree

Takes in a size and a set of flags

Architecture dependent implementations:
- get_zone
    - Converts a zone passed in into a new zone that is used for buddy allocation
- get_zone_fallback
    - Returns a fallback zone for an zone
    - Can fail (Return ZONE_NULL)
- get_zone_info
    - Returns some zone info based on an zone

ZONE_NULL: 0xFFFF

