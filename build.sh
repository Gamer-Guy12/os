dd if=/dev/zero of=os.img bs=1M count=256

parted os.img --script -- \
  mklabel gpt \
  mkpart ESP fat32 1MiB 10MiB \
  set 1 esp on \
  mkpart BOOT ext2 10MiB 100%

dd if=/dev/zero of=fat32.img bs=1M count=9
mkfs.fat -F 32 fat32.img

dd if=/dev/zero of=ext2.img bs=1M count=246
mkfs.ext2 ext2.img

e2mkdir ext2.img:/boot/grub

e2cp ../../build/bin/kernel.bin ext2.img:/boot/
e2cp ../../targets/x86_64/grub.cfg ext2.img:/boot/grub/

grub-mkstandalone \
  --format=x86_64-efi \
  --output=BOOTX64.EFI \
  --install-modules="normal part_gpt fat ext2 multiboot2 configfile" \
  --locales="" \
  --fonts="" \
  "boot/grub/grub.cfg=<(echo 'configfile (hd0,gpt2)/boot/grub/grub.cfg')"

mmd -i fat32.img ::/EFI
mmd -i fat32.img ::/EFI/BOOT
mcopy -i fat32.img BOOTX64.EFI ::/EFI/BOOT

dd if=fat32.img of=os.img bs=512 seek=2048 conv=notrunc
dd if=ext2.img of=os.img bs=512 seek=20480 conv=notrunc
