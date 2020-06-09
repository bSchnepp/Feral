include ../../Makerules

CFLAGS += -I./inc -I../inc -I../../inc -I../ -I. -I../../

LOCAL_FILES != ls ./*.c

all:
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(LOCAL_FILES)
	mv *.o ../

all-efi:
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) -D FERAL_BUILD_STANDALONE_UEFI_ $(LOCAL_FILES)
	mv *.o ../
clean:
	rm -rf ./*.o
