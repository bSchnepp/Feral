include ../../../Makerules

FILES += vga/*.c mm/*.c archstart.c
CFLAGS += -I../inc -I../ -I../../ -I../../../ -I../../../kern/inc -I../../../inc

all:
	$(AS) -felf64 boot/helper.asm
	$(AS) -felf64 boot/init.asm -o init_mb2.o
	$(AS) -felf64 boot/init64.asm -o init64.o
	$(CC) $(CFLAGS) $(FILES)
	mv ./*.o ./boot/*.o ../../../
	
clean:
	rm -rf *.o
	rm -rf boot/*.o
