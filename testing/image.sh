#!/bin/bash
# Delete old files
rm BOOTX64.EFI
rm os.img
rm fat32.img
rm ext2.img

# Create image

dd if=/dev/zero of=os.img bs=1M count=256

# Create esp and other thingy
dd if=/dev/zero of=fat32.img bs=1M count=9
dd if=/dev/zero of=ext2.img bs=512 count=501760

# Format disks
parted os.img --script -- \
  mklabel gpt \
  mkpart ESP fat32 1MiB 10MiB \
  set 1 esp on \
  mkpart BOOT ext2 10MiB 100%

mkfs.fat -F 32 fat32.img
mkfs.ext2 ext2.img

# Copy kernel and grub cfg
e2mkdir ext2.img:/boot/grub/
e2cp ../build/bin/kernel.bin ext2.img:/boot/
e2cp ../targets/x86_64/grub.cfg ext2.img:/boot/grub/

# Create efi binary
grub-mkstandalone -O x86_64-efi -o BOOTX64.EFI "boot/grub/grub.cfg=grub.cfg"

# Copy efi binary onto fat32
mmd -i fat32.img ::/EFI
mmd -i fat32.img ::/EFI/BOOT

mcopy -i fat32.img BOOTX64.EFI ::/EFI/BOOT

# Copy the config onto fat32
mmd -i fat32.img ::/grub
mcopy -i fat32.img grub.cfg ::/grub/

# Put disk together
dd if=fat32.img of=os.img bs=512 seek=2048 conv=notrunc
dd if=ext2.img of=os.img bs=512 seek=20480 conv=notrunc
