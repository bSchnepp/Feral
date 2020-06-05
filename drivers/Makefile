include ../Makerules

all:	device proc
	# $(CC) $(CFLAGS) devices.o -o ../drivers.o
	mv devices.o ../driver.o

.PHONY:	all

device:
	cd devices && $(MAKE)

proc:
	cd proc && $(MAKE)

clean:
	rm -rf ./*.o
	cd devices && $(MAKE) clean
	# cd gpu && $(MAKE) clean
	# cd proc && $(MAKE) clean
	# cd wayapi && $(MAKE) clean

