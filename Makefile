MAKE = bmake

include Makerules
include arch/$(ARCH)/Archrules.mk


INCLUDES += -Ikern/inc
VGA_FILES != ls arch/$(ARCH)/vga/*.c

# We still require GNU GRUB and xorriso for testing though...
.PHONY:	all clean run iso kernel qemu qemu-nokvm

all:	kernel

kernel:
	mkdir -p build/$(ARCH)/	
	
	cd arch && $(MAKE)
#	cd proc && $(MAKE) 
#	cd mm && $(MAKE) 
	cd io && $(MAKE) 
	cd drivers && $(MAKE)
	cd kern && $(MAKE)
#	cd sec && $(MAKE)
	
	# libmm.a libprocmgr.a 
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) ./io/*.c -o ./iofuncs.o
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(VGA_FILES) -o ./*.o kern/kernel_main.o kern/krnlfuncs.o kern/krnlfuncs.o kern/krnl_private.o kern/objmgr.o
	$(LD) -T $(LINKIN) -o $(KERNEL) ./*.o ./kern/*.o

iso:	kernel
	mkdir -p build/isofiles/boot/grub
	cp $(KERNEL) build/isofiles/boot
	cp arch/$(ARCH)/grub.cfg build/isofiles/boot/grub
	grub-mkrescue --verbose --output=$(ISO) build/isofiles 2> /dev/null
	rm -rf build/$(ARCH)

clean:
	rm -rf build/
	rm -rf ./*.o
	rm -rf ./*.a
	rm -rf bootloader/*.o bootloader/*.EFI
	
	cd kern && $(MAKE) clean
	cd sec && $(MAKE) clean
#	cd proc && $(MAKE)  clean && cd ../mm && $(MAKE) clean
	## TODO: clean up object files too.

qemu:	iso
	qemu-system-$(ARCH) $(CPU) -cdrom $(ISO) -smp 2 -m 6G --enable-kvm  -d int,cpu_reset -no-reboot -no-shutdown # We enable KVM to access the features of the ZEN version 1.... we'll need to change this when we're (eventually) self-hosting.

qemu-nokvm:	iso
	qemu-system-$(ARCH) $(CPU) -cdrom $(ISO) -m 2G -d int,cpu_reset -no-reboot -no-shutdown

# The most alien CPU we can get.
qemu-nokvm-unsupportedcpu:	iso
	qemu-system-$(ARCH) -cpu KnightsMill -cdrom $(ISO) -smp 2 -m 6G
	
qemu-lldb:	iso
	qemu-system-$(ARCH) $(CPU) -cdrom $(ISO) -smp 2 -m 6G -S -s -d int,cpu_reset -no-reboot&	
	
qemu-efi:
	mkdir -p build/$(ARCH)/
	mkdir -p build/EFI/Boot
	mkdir -p build/EFI/Feral	
	
	cd arch && $(MAKE)
#	cd proc && $(MAKE) 
#	cd mm && $(MAKE) 
	cd io && $(MAKE) 
	cd drivers && $(MAKE)
	cd kern && $(MAKE)
	cd sec && $(MAKE)
	
	# libmm.a libprocmgr.a 
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) -DFERAL_BUILD_STANDALONE_UEFI_ ./io/*.c -o ./iofuncs.o
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) -DFERAL_BUILD_STANDALONE_UEFI_ $(VGA_FILES) -o ./*.o kern/kernel_main.o kern/krnlfuncs.o kern/krnlfuncs.o kern/krnl_private.o kern/objmgr.o
	$(LD) -T $(LINKIN) -o $(KERNEL) ./*.o ./kern/*.o
	
	cp $(EFI_CODE) ./efi.bin
	cd bootloader && $(MAKE) all && cp BOOTX64.EFI ../ && $(MAKE) clean && cd ..
	mv BOOTX64.EFI build/EFI/Boot/BOOTX64.EFI
	
	# Instead of a normal ISO, we pretend the build directory is an ESP.
	cp $(KERNEL) build/EFI/Feral/FERALKER.NEL
	qemu-system-$(ARCH) $(CPU) -smp 2 -m 6G --enable-kvm -pflash ./efi.bin -hda fat:rw:build -net none
	rm -rf ./efi.bin
	

qemu-nokvm-efi:	iso
	cp $(EFI_CODE) ./efi.bin
	qemu-system-$(ARCH) $(CPU) -cdrom $(ISO) -smp 2 -m 6G -pflash  ./efi.bin
	rm -rf ./efi.bin
