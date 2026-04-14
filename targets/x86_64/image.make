IMAGE_NAME:=build/os.img

.PHONY: image 
image: tools/limine/limine $(IMAGE_NAME)

$(IMAGE_NAME): build/bin/kernel.bin
	dd if=/dev/zero of=$(IMAGE_NAME) bs=1M count=64
	# Thank you limine source
	PATH=$$PATH:/usr/sbin:/sbin sgdisk $(IMAGE_NAME) -n 1:2048 -t 1:ef00 -m 1
	mformat -i $(IMAGE_NAME)@@1M
	mmd -i $(IMAGE_NAME)@@1M ::/EFI ::/EFI/BOOT ::/boot ::/boot/limine
	mcopy -i $(IMAGE_NAME)@@1M build/bin/kernel.bin ::/boot
	mcopy -i $(IMAGE_NAME)@@1M targets/x86_64/limine.conf ::/boot/limine
	mcopy -i $(IMAGE_NAME)@@1M tools/limine/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i $(IMAGE_NAME)@@1M assets ::/boot

