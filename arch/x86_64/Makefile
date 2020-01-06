include ../../Makerules

FILES += vga/*.c
CFLAGS += -I../inc

all:
	$(AS) -felf64 boot/helper.asm
	mv ./boot/*.o ../../

mb2-extra:
	$(AS) -felf64 boot/init.asm
	mv ./boot/*.o ../../
	
clean:
	rm -rf *.o
