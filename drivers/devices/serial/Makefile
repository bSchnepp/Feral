include ../../../Makerules

all:
	$(CC) $(CFLAGS) -I../../../inc serial.c -I../../../kern/inc -o ../serial.o

clean:
	rm -rf ../serial
	rm -rf ./*.o
	rm -rf serial.o
