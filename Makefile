MAKE       = bmake # Insist on BSD make. This way we can distribute a self-hosting system without worrying about GPL issues. 
		   # (I don't want to be emailed every 15 seconds for source code requests, and certainly don't want to be 
		   # forced to package source for an insignifigant component with _every_ distribution). Also
                   # so that Feral can be used by just about anyone for just about any purpose they 
                   # want. (Fork the whole thing, modify as needed, all that stuff. Do not care about closed-source forks.)

		   # We still do use GNU xorriso and GNU GRUB though, so we'll have to eventually write a UEFI-capable Multiboot 2 bootloader and an ISO tool as well.

		   # Waypoint could also be used on systems which really shouldn't be compiling GNU stuff as part of the install process (RISC-V based video game consoles???).
		   # Compiling something takes much longer than wgetting the binary and slapping it in /usr/bin, anyway. Wouldn't want to annoy console people.
		   # (Especially if this ends up on a mobile console/laptop/something! That'd *REALLY* drain the battery.)

GDB        = lldb # Use LLVM debugger when we can, if we make a makerule for this in the future.

ARCH       = x86_64
KERNEL    := build/$(ARCH)/FERALKER.NEL
ISO       := build/WAYPOINT.ISO

LINKIN    := arch/$(ARCH)/linkin.lld
GRUB_CFG  := arch/$(ARCH)/grub.cfg

AS        := yasm       # We use yasm to assemble.
CC        := clang      # We use clang to compile.
CXX       := clang++    # We may use C++ for something later. (Would only ever really be a "C with classes" anyway.)
LD        := ld.lld

TARGET     = -target $(ARCH)-pc-none-gnu
ASM_FILES != ls arch/$(ARCH)/boot/*.asm
VGA_FILES != ls arch/$(ARCH)/vga/*.c
KERN_MAIN != ls kern/*.c

INCLUDES  := inc
CFLAGS    := -std=c11 -ffreestanding -nostdlib -O2 -Wall -Wextra -mno-red-zone -c -v -mno-mmx -mno-sse -msoft-float
# -c prevents linking (important!!!) and -v for verbosity.

# We don't really care about 'Entering directory...'.
MAKEFLAGS += --no-print-directory

# We still require GNU GRUB and xorriso for testing though...
.PHONY:	all clean run iso kernel qemu qemu-nokvm

all:	kernel

kernel:
	# ok, im being lazy, since this doesn't really matter and we only have one assembly file.
	mkdir -p build/$(ARCH)/	
	$(AS) -felf64 $(ASM_FILES) -o multiboot.o
	cd proc && make && cd ../mm && make
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(VGA_FILES) -o vga.o libmm.a libprocmgr.a
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(KERN_MAIN)
	$(LD) -T $(LINKIN) -o $(KERNEL) ./*.o

iso:	kernel
	mkdir -p build/isofiles/boot/grub
	cp $(KERNEL) build/isofiles/boot
	cp arch/$(ARCH)/grub.cfg build/isofiles/boot/grub
	grub-mkrescue -o $(ISO) --verbose build/isofiles 2> /dev/null
	rm -rf build/$(ARCH)

clean:
	rm -rf build/
	rm -rf ./*.o
	rm -rf ./*.a
	cd proc && make clean && cd ../mm && make clean
	## TODO: clean up object files too.

qemu:	iso
	qemu-system-x86_64 -cdrom $(ISO) --enable-kvm  # We enable KVM to access the features of the ZEN version 1.... we'll need to change this when we're (eventually) self-hosting.

qemu-nokvm:	iso
	qemu-system-x86_64 -cdrom $(ISO) # --enable-kvm  # We enable KVM to access the features of the ZEN version 1.... we'll need to change this when we're (eventually) self-hosting.
	
