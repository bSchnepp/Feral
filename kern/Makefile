## TODO

include ../Makerules

CFLAGS += -I./inc -I../inc -I../ -I.

KERN_FILES != ls ./*.c mm/*.c arch/$(ARCH)/*.c ../sec/*.c

KERN_ASM != ls arch/$(ARCH)/*.asm

all:
	cd entry && $(MAKE) all && cd ..
	$(AS) -felf64 $(KERN_ASM) -o kernasm.o
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(KERN_FILES)

all-efi:
	cd entry && $(MAKE) all-efi && cd ..
	$(AS) -felf64 $(KERN_ASM) -o kernasm.o
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) -D FERAL_BUILD_STANDALONE_UEFI_ $(KERN_FILES)
	
clean:
	rm -rf ./*.o
