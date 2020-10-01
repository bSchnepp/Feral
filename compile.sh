#!/bin/sh

rm -rf build && mkdir -p build && cd build && cmake .. -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug && make -j32
mv FERALKER FERALKER.NEL
mkdir -p isofiles/boot/grub
cp  FERALKER.NEL isofiles/boot
cp ../arch/x86_64/grub.cfg isofiles/boot/grub
grub-mkrescue --verbose --output=WAYPOINT.ISO isofiles 2> /dev/null
