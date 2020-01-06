## TODO

include ../Makerules

CFLAGS += -I./inc -I../inc -I../ -I.

KERN_FILES != ls ./*.c ./mb2/*.c mm/*.c arch/$(ARCH)/*.c ../sec/*.c

KERN_ASM != ls arch/$(ARCH)/*.asm

all:
	$(AS) -felf64 $(KERN_ASM) -o kernasm.o
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(KERN_FILES) kernasm.o
	
clean:
	rm -rf ./*.o
