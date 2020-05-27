## TODO

include ../Makerules

CFLAGS += -I./inc -I../inc -I../ -I.

KERN_FILES != ls ./*.c ./**/*.c mm/*.c arch/$(ARCH)/*.c ../sec/*.c

KERN_ASM != ls arch/$(ARCH)/*.asm

all:
	cd arch && $(MAKE) all && cd ..
	cd entry && $(MAKE) all && cd ..
	$(AS) -felf64 $(KERN_ASM) -o kernasm.o
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(KERN_FILES)
	mv *.o ../

all-efi:
	cd arch && $(MAKE) all && cd ..
	cd entry && $(MAKE) all-efi && cd ..
	$(AS) -felf64 $(KERN_ASM) -o kernasm.o
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) -D FERAL_BUILD_STANDALONE_UEFI_ $(KERN_FILES)
	mv *.o ../
	
clean:
	rm -rf ./*.o
