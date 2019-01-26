MAKE = bmake
INCLUDE = -I../inc

all:
	clang -O3 -mtune=native -march=native $(INCLUDE) ./kconfig.c -o ../kconfig.elf

install:
	mv ../kconfig.elf /usr/local/bin/kconfig.elf

clean:
	rm -rf ../kconfig.elf
