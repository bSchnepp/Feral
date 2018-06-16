CC = clang -target x86_64-pc-win32-coff
CFLAGS = -I../inc/efi -fno-stack-protector -fshort-wchar -mno-red-zone -c -I../inc -nostdlib

LD = x86_64-w64-mingw32-ld
LDFLAGS = -s -Bsymbolic -nostdlib -shared -e uefi_main --subsystem 10


all:
	$(CC) $(CFLAGS) loader.c -o LOADER.o
	$(LD) $(LDFLAGS) LOADER.o -o BOOTX64.EFI

clean:
	rm -rf LOADER.o BOOTX64.EFI
