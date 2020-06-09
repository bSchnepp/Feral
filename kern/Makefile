## TODO

include ../Makerules

CFLAGS += -I./inc -I../inc -I../ -I.

KERN_FILES != ls ./*.c ./**/*.c mm/*.c arch/$(ARCH)/*.c ../sec/*.c

all:
	cd arch && $(MAKE) all && cd ..
	cd entry && $(MAKE) all && cd ..
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(KERN_FILES)
	mv *.o ../

all-efi:
	cd arch && $(MAKE) all && cd ..
	cd entry && $(MAKE) all-efi && cd ..
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) -D FERAL_BUILD_STANDALONE_UEFI_ $(KERN_FILES)
	mv *.o ../
	
clean:
	cd arch && $(MAKE) clean
	rm -rf ./*.o
