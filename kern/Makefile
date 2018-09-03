## TODO

include ../Makerules

CFLAGS += -I./inc -I../inc

KERN_FILES != ls ./*.c

all:
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(KERN_FILES)
	
clean:
	rm -rf ./*.o
