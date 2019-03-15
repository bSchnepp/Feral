## TODO

include ../Makerules

CFLAGS += -I./inc -I../inc -I../

KERN_FILES != ls ./*.c mm/createpage.c

KERN_ASM = arch/$(ARCH)/mm.asm

all:
	$(AS) -felf64 $(KERN_ASM) -o kernasm.o
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(KERN_FILES) kernasm.o
	
clean:
	rm -rf ./*.o
