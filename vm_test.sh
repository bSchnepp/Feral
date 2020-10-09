#!/bin/sh

./gen_testfile.sh
rm -rf build && mkdir -p build && cd build && cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug && make -j32
if [ $? -eq 0 ]; then
	mv FERALKER FERALKER.NEL
	mkdir -p isofiles/boot/grub
	cp  FERALKER.NEL isofiles/boot
	cp ../arch/x86_64/grub.cfg isofiles/boot/grub
	grub-mkrescue --verbose --output=WAYPOINT.ISO isofiles 2> /dev/null
	qemu-system-x86_64 -cpu EPYC -cdrom WAYPOINT.ISO -smp 2 -m 6G -d int,cpu_reset -no-reboot -no-shutdown
else
	echo "Build failed."
fi