CC = clang -target x86_64-pc-unknown-windows
CFLAGS = -I../inc/efi -fno-stack-protector -fshort-wchar -mno-red-zone -c -I.. -I../inc -I../kern/inc -nostdlib -fshort-wchar

LD = ld.lld -flavor link
LDFLAGS = -subsystem:efi_application /nodefaultlib --nostdlib -nostdlib


all:
	$(CC) $(CFLAGS) -g3 boot.c -o LOADER.o
	# Illustrious hack to make lld work. Hence why llvm toolchain, not gnu.
	$(LD) $(LDFLAGS) LOADER.o /entry:uefi_main /out:BOOTX64.EFI

clean:
	rm -rf LOADER.o BOOTX64.EFI
