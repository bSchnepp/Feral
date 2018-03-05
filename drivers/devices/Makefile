include ../../Makerules

all:	serial
	# $(CC) $(CFLAGS) serial.o -o ../devices.o
	# cp serial.o ../devices.o
	mv serial.o ../devices.o

.PHONY:	serial clean

serial:
	cd serial && make

clean:
	rm -rf ./*.o
	cd serial && make clean
