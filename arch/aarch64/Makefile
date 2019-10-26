# The aarch64 port is tested *exclusively* in QEMU, and may or may not work on real hardware.
# (Use on real hardware at your own risk)

AS := clang -target aarch64-none-eabi

# TODO: Specific CPU selection
all:
	$(AS) -fpic -ffreestanding -c boot/boot.S -o ../../multiboot.o
