include ../../Makerules

CFLAGS += -I./inc -I../inc -I../../inc -I../ -I. -I../../

LOCAL_FILES != ls ./*.c
LOCAL_ASM != ls arch/$(ARCH)/*.asm

all:
	$(AS) -felf64 $(LOCAL_ASM) -o kernentryasm.o
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(LOCAL_FILES) kernentryasm.o
	mv *.o ../

all-efi:
	$(AS) -felf64 $(LOCAL_ASM) -o kernentryasm.o
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) -D FERAL_BUILD_STANDALONE_UEFI_ $(LOCAL_FILES) kernentryasm.o
	mv *.o ../
clean:
	rm -rf ./*.o
