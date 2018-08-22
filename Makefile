MAKE = bmake

include Makerules

VGA_FILES != ls arch/$(ARCH)/vga/*.c
KERN_MAIN != ls kern/*.c

EFI_CODE = /usr/share/ovmf/ovmf_code_x64.bin

# We still require GNU GRUB and xorriso for testing though...
.PHONY:	all clean run iso kernel qemu qemu-nokvm

all:	kernel

kernel:
	mkdir -p build/$(ARCH)/	
	
	cd arch && make
#	cd proc && make 
#	cd mm && make 
	cd io && make 
	cd drivers && make
	
	# libmm.a libprocmgr.a 
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(VGA_FILES) -o vga.o io.o driver.o
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(KERN_MAIN)
	$(LD) -T $(LINKIN) -o $(KERNEL) ./*.o

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
#	cd proc && make  clean && cd ../mm && make clean
	## TODO: clean up object files too.

qemu:	iso
	qemu-system-x86_64 -cdrom $(ISO) -smp 2 -m 1G --enable-kvm  # We enable KVM to access the features of the ZEN version 1.... we'll need to change this when we're (eventually) self-hosting.

qemu-nokvm:	iso
	qemu-system-x86_64 -cdrom $(ISO) -smp 2 -m 1G
	
	
qemu-efi:	iso
	cp $(EFI_CODE) ./efi.bin
	qemu-system-x86_64 -cdrom $(ISO) -smp 2 -m 1G --enable-kvm  -pflash ./efi.bin
	rm -rf ./efi.bin
	

qemu-nokvm-efi:	iso
	cp $(EFI_CODE) ./efi.bin
	qemu-system-x86_64 -cdrom $(ISO) -smp 2 -m 1G -pflash  ./efi.bin
	rm -rf ./efi.bin
