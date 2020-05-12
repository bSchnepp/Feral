include ../../Makerules

FILES += vga/*.c archstart.c
CFLAGS += -I../inc -I../ -I../../ -I../../../ -I../../kern/inc -I../../inc

all:
	$(AS) -felf64 boot/helper.asm
	$(AS) -felf64 boot/init64.asm
	$(CC) $(CFLAGS) $(FILES)
	mv ./*.o ./boot/*.o ../../

mb2-extra:
	$(AS) -felf64 boot/init.asm
	mv ./boot/*.o ../../
	
clean:
	rm -rf *.o
