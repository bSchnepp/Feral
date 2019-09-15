include ../../Makerules

FILES += vga/*.c
CFLAGS += -I../inc

all:
	$(AS) -felf64 boot/helper.asm
	$(AS) -felf64 boot/init.asm
	mv ./boot/*.o ../../
	
clean:
	rm -rf *.o
