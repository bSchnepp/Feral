CC = clang -target x86_64-pc-win32-coff
CFLAGS = -I../inc/efi -fno-stack-protector -fshort-wchar -mno-red-zone -c -I../inc -I../kern/inc -nostdlib

LD = ld.lld -flavor link
LDFLAGS = -subsystem:efi_application -nodefaultlib


all:
	$(CC) $(CFLAGS) loader.c -o LOADER.o
	# Illustrious hack to make lld work. Hence why llvm toolchain, not gnu.
	$(LD) $(LDFLAGS) LOADER.o /entry:uefi_main /out:BOOTX64.EFI

clean:
	rm -rf LOADER.o BOOTX64.EFI
