KERNEL := kernel.elf
ISO := vibeos.iso

CC := gcc
LD := ld

CFLAGS := -Wall -Wextra -O2 -pipe \
          -ffreestanding -fno-stack-protector -fno-stack-check \
          -fno-lto -fno-pie -fno-pic -m64 -march=x86-64 \
          -mno-80387 -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel \
          -I./limine

LDFLAGS := -nostdlib -static -pie --no-dynamic-linker -z text -z max-page-size=0x1000 -T src/linker.ld

.PHONY: all limine run clean

all: $(ISO)

limine:
	@if [ ! -f "limine/limine" ]; then \
		make -C limine; \
	fi

src/kernel.o: src/kernel.c limine
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): src/kernel.o src/linker.ld
	$(LD) $(LDFLAGS) src/kernel.o -o $@

$(ISO): $(KERNEL) limine limine.cfg
	rm -rf iso_root
	mkdir -p iso_root
	cp $(KERNEL) limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/
	mkdir -p iso_root/EFI/BOOT
	cp limine/BOOTX64.EFI iso_root/EFI/BOOT/
	cp limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -b limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(ISO)
	./limine/limine bios-install $(ISO)

run: $(ISO)
	qemu-system-x86_64 -m 2G -M q35 -cdrom $(ISO) -boot d

clean:
	rm -rf src/*.o $(KERNEL) $(ISO) iso_root limine
