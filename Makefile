MAKE = bmake

include Makerules
include arch/$(ARCH)/Archrules.mk


INCLUDES += -Ikern/inc
VGA_FILES != ls kern/arch/$(ARCH)/vga/*.c

# We still require GNU GRUB and xorriso for testing though...
.PHONY:	all clean run iso kernel qemu qemu-nokvm

all:	kernel

kernel:
	mkdir -p build/$(ARCH)/	
	
	cd drivers && $(MAKE)
	cd kern && $(MAKE)
	 
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(VGA_FILES)
	$(LD) -T $(LINKIN) -o $(KERNEL) ./*.o
	#cd tests && $(MAKE) all
	
kernel-efi:
	mkdir -p build/$(ARCH)/
	mkdir -p build/EFI/Boot
	mkdir -p build/EFI/Feral
	
	cd drivers && $(MAKE)
	cd kern && $(MAKE) all-efi
	 
	$(CC) $(TARGET) -I$(INCLUDES) -DFERAL_BUILD_STANDALONE_UEFI_ $(CFLAGS) $(VGA_FILES)
	$(LD) -T $(LINKIN_EFI) -o $(KERNEL) ./*.o
	#strip $(KERNEL)
	#cd tests && $(MAKE) all
	
img-efi: 	kernel-efi
	mkdir -p build/EFI/Feral
	cp $(KERNEL) build/EFI/Feral/FERALKER.NEL
	cd bootloader && $(MAKE) all && cp BOOTX64.EFI ../ && $(MAKE) clean && cd ..
	mv BOOTX64.EFI build/EFI/Boot/BOOTX64.EFI
	

iso:	kernel
	mkdir -p build/isofiles/boot/grub
	#strip $(KERNEL)
	cp $(KERNEL) build/isofiles/boot
	cp arch/$(ARCH)/grub.cfg build/isofiles/boot/grub
	grub-mkrescue --verbose --output=$(ISO) build/isofiles 2> /dev/null
	rm -rf build/$(ARCH)

iso-mb2-efi:	kernel
	mkdir -p build/isofiles/boot/grub
	strip $(KERNEL)
	cp $(KERNEL) build/isofiles/boot
	cp arch/$(ARCH)/grub.cfg build/isofiles/boot/grub
	grub-mkrescue --verbose --output=$(ISO) build/isofiles 2> /dev/null
	rm -rf build/$(ARCH)

clean:
	rm -rf build/
	rm -rf ./*.o
	rm -rf ./*.a
	rm -rf bootloader/*.o bootloader/*.EFI
	rm -rf efi.bin
	
	cd kern && $(MAKE) clean
	cd sec && $(MAKE) clean
	cd tests && $(MAKE) clean

qemu:	iso
	qemu-system-$(ARCH) $(CPU) -cdrom $(ISO) -smp 2 -m 6G --enable-kvm -d int,cpu_reset -no-reboot -no-shutdown # We enable KVM to access the features of the ZEN version 1.... we'll need to change this when we're (eventually) self-hosting.

qemu-mb2-efi:	iso-mb2-efi
	cp $(EFI_CODE) ./efi.bin
	qemu-system-$(ARCH) $(CPU) -cdrom $(ISO) -smp 2 -m 6G --enable-kvm  -pflash ./efi.bin -d int,cpu_reset -no-reboot -no-shutdown # We enable KVM to access the features of the ZEN version 1.... we'll need to change this when we're (eventually) self-hosting.
	rm -rf ./efi.bin

qemu-nokvm:	iso
	qemu-system-$(ARCH) $(CPU) -cdrom $(ISO) -m 2G -d int,cpu_reset -no-reboot -no-shutdown

# The most alien CPU we can get.
qemu-nokvm-unsupportedcpu:	iso
	qemu-system-$(ARCH) -cpu KnightsMill -cdrom $(ISO) -smp 2 -m 6G
	
qemu-lldb:	iso
	qemu-system-$(ARCH) $(CPU) --enable-kvm -cdrom $(ISO) -smp 2 -m 6G -S -s -d int,cpu_reset -no-reboot&	
	
qemu-nokvm-lldb:	iso
	qemu-system-$(ARCH) $(CPU) -cdrom $(ISO) -smp 2 -m 6G -S -s -d int,cpu_reset -no-reboot&	


qemu-efi: 	img-efi
	cp $(EFI_CODE) ./efi.bin
	
	# Instead of a normal ISO, we pretend the build directory is an ESP.
	qemu-system-$(ARCH) $(CPU) -smp 2 -m 6G --enable-kvm -pflash ./efi.bin -hda fat:rw:build -net none -d int,cpu_reset -no-reboot -no-shutdown
	rm -rf ./efi.bin

qemu-efi-alt: 	img-efi
	cp $(EFI_CODE) ./efi.bin
	
	# Instead of a normal ISO, we pretend the build directory is an ESP.
	qemu-system-$(ARCH) $(CPU) -smp 2 -m 6G -pflash ./efi.bin -hda fat:rw:build -net none -d int -no-reboot -no-shutdown
	rm -rf ./efi.bin
	
qemu-efi-lldb:	img-efi
	cp $(EFI_CODE) ./efi.bin
	qemu-system-$(ARCH) --enable-kvm -smp 2 -m 6G -pflash ./efi.bin -hda fat:rw:build -net none -S -s -d int,cpu_reset -no-reboot -no-shutdown
	rm -rf ./efi.bin

qemu-nokvm-efi:	img-efi
	cp $(EFI_CODE) ./efi.bin
	qemu-system-$(ARCH) $(CPU) -smp 2 -m 6G -pflash ./efi.bin -hda fat:rw:build -net none
	rm -rf ./efi.bin

qemu-nokvm-efi-lldb:	img-efi
	cp $(EFI_CODE) ./efi.bin
	qemu-system-$(ARCH) $(CPU) -smp 2 -m 6G -pflash ./efi.bin -hda fat:rw:build -net none -S -s -d int,cpu_reset -no-reboot -no-shutdown
	rm -rf ./efi.bin

qemu-nokvm-unsupportedcpu-efi:	img-efi
	cp $(EFI_CODE) ./efi.bin
	qemu-system-$(ARCH) -cpu KnightsMill -smp 2 -m 6G -pflash ./efi.bin -hda fat:rw:build -net none -d int,cpu_reset -no-reboot -no-shutdown
	rm -rf ./efi.bin
	
qemu-nokvm-unsupportedcpu-efi-lldb:	img-efi
	cp $(EFI_CODE) ./efi.bin
	qemu-system-$(ARCH) -cpu KnightsMill -smp 2 -m 6G -pflash ./efi.bin -hda fat:rw:build -net none -S -s -d int,cpu_reset -no-reboot -no-shutdown
	rm -rf ./efi.bin
