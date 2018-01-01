MAKE       = bmake # Insist on BSD make, as we want to keep GPL out of base as much as possible 
                   # so that Feral can be used by just about anyone for just about any purpose they 
                   # want.

GDB        = lldb # Use LLVM debugger when we can, if we make a makerule for this in the future.

ARCH       = x86_64
KERNEL    := build/$(ARCH)/FERALKER.NEL
ISO       := build/WAYPOINT.ISO

LINKIN    := arch/$(ARCH)/linkin.lld
GRUB_CFG  := arch/$(ARCH)/grub.cfg

AS        := yasm       # We use yasm to assemble.
CC        := clang      # We use clang to compile.
CXX       := clang++    # We may use C++ for something later.
RUSTC     := rustc      # TODO: Check for nightly. We'll write memory management in Rust.
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
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(VGA_FILES) -o vga.o
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
	## TODO: clean up object files too.

qemu:	iso
	qemu-system-x86_64 -cdrom $(ISO) --enable-kvm  # We enable KVM to access the features of the ZEN version 1.... we'll need to change this when we're (eventually) self-hosting.

qemu-nokvm:	iso
	qemu-system-x86_64 -cdrom $(ISO) # --enable-kvm  # We enable KVM to access the features of the ZEN version 1.... we'll need to change this when we're (eventually) self-hosting.
	
