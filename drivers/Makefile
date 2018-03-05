include ../Makerules

all:	device gpu proc wayapi
	# $(CC) $(CFLAGS) devices.o -o ../drivers.o
	mv devices.o ../driver.o

.PHONY:	all

device:
	cd devices && make

gpu:
	cd gpu && make

proc:
	cd proc && make

wayapi:
	cd wayapi && make

clean:
	rm -rf ./*.o
	cd devices && make clean
	# cd gpu && make clean
	# cd proc && make clean
	# cd wayapi && make clean

