include ../Makerules

FILES += $(ARCH)/vga/*.c
CFLAGS += -I../inc

ASM_FILES = $(ARCH)/boot/*.asm


all:
	$(AS) -felf64 $(ASM_FILES) -o ../multiboot.o
