include ../Makerules

CFLAGS += -I./inc -I../inc -I../ -I../kern/inc

SEC_FILES != ls ./*.c

all:
	$(CC) $(TARGET) -I$(INCLUDES) $(CFLAGS) $(SEC_FILES)
	
clean:
	rm -rf ./*.o
