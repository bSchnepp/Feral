#!/bin/sh

rm -rf tests/all.c
find kern/mm -name '*.c' -exec cat {} \; >> tests/all.c
find kern -maxdepth 1 -name '*.c' -exec cat {} \; >> tests/all.c
find sec/ -name '*.c' -exec cat {} \; >> tests/all.c
find drivers/ -name '*.c' -exec cat {} \; >> tests/all.c
