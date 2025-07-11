# OS
OS?

The kernel must be built aligned to 2 MB boundires cuz of paging and stuff that im too lazy to do

virbuild.sh is a script to build for physical memory extension or smth

QEMU flags: -cdrom os.iso -m 4096 -d int,cpu_reset -no-reboot

This is hybrid bootable but only supports uefi if u understand
