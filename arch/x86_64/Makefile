include ../../Makerules

FILES += vga/*.c
CFLAGS += -I../inc -I../ -I../../ -I../../../

all:
	$(AS) -felf64 boot/helper.asm
	mv ./boot/*.o ../../

mb2-extra:
	$(AS) -felf64 boot/init.asm
	mv ./boot/*.o ../../
	
clean:
	rm -rf *.o
