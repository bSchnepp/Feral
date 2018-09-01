include ../../Makerules

FILES += vga/*.c
CFLAGS += -I../inc

ASM_FILES = boot/*.asm


all:
	$(AS) -felf64 $(ASM_FILES) -o ../../multiboot.o
