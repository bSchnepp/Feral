## TODO

include ../Makerules

CFLAGS += -I./inc -I../inc -I../

KERN_FILES != ls ./*.c mm/${ARCH}/createpage.c

all:
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(KERN_FILES)
	
clean:
	rm -rf ./*.o
