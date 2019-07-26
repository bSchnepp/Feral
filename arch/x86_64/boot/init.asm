section .multiboot2
align 4

MULTIBOOT_MAGIC EQU 0xE85250D6
MULTIBOOT_ARCH_ EQU 0x00000000
MULTIBOOT_SIZE_ EQU (header_end - header_start)

header_start:

	dd MULTIBOOT_MAGIC			; We're multiboot2.
	dd MULTIBOOT_ARCH_			; We're on the x86 arch.
	dd MULTIBOOT_SIZE_			; Size of the header.

	; Try to avoid negative numbers in general (they can cause compiler issues in rare, specific cases.)
	dd 0x100000000 - (MULTIBOOT_MAGIC + MULTIBOOT_ARCH_ + MULTIBOOT_SIZE_)

	; Optional Multiboot2 tags should show up here... we'll probably ignore them anyway.

	; This end structure is required by Multiboot.
	dw 0
	dw 0
	dd 8

header_end:


GLOBAL _start
SECTION .text
BITS 32

; This is the blue box of death. We'll worry about a useful panic() later.
; The real panic() (KeInternalWarn() for warning, KeStopError() for actual 'panic()') later.
boot_panic:
	mov dword [0xB8000], 0x1F001F00		; Address b8000 is VGA memory. As such, by dumping some data there (1F001F00), we create the "blue box of death".
	mov dword [0xB8004], 0x1F001F00
	mov dword [0xB80A0], 0x1F001F00
	mov dword [0xB80A4], 0x1F001F00
	cli
	hlt
	jmp $
	
boot_panic_invalid_arch:
	mov dword [0xB8000], 0x4F004F00		; If attempting to boot on an IA-32, we'll show the red box of death using the same thing above.
	mov dword [0xB8004], 0x4F004F00
	mov dword [0xB80A0], 0x4F004F00
	mov dword [0xB80A4], 0x4F004F00
	cli
	hlt
	jmp $
	

global _start
_start:
	mov esp, stack_top	; Set the stack up.

	; Check for multiboot 2 compliance.
	; This number is the multiboot2 magic number: if this is present, then the kernel was booted correctly.
	; If it was not, then something went wrong, and we'll panic.
	sub eax, 0x36D76289	; use sub over cmp when we have an excuse for it.
	jnz boot_panic
	
	; Push ebx for when we need it later. (holds multiboot struct)
	mov [multiboot_value], ebx

	; Check CPUID...
	pushfd 
	pop eax

	; Use ECX to check the CPUID value.
	mov ecx, eax
	
	; Now we flip the CPUID bit.
	xor eax, 1 << 21

	; Push flags back.
	push eax
	popfd

	; Now pop flags back into eax.
	pushfd
	pop eax

	; And check if the bit was actually flipped.
	cmp eax, ecx
	je boot_panic	; If these are the same, the bit was not flipped.
	
	push ecx
	popfd

	; If we've reached here, that means we were booted by multiboot, and that the hardware supports CPUID.
	; We *could* be lazy and instead assume that the CPU does support CPUID (why would you want this on an 8086???), but we can't assume what CPU this runs on.

	mov eax, 0x80000000	; Long mode argument for CPUID.
	cpuid
	
	cmp eax, 0x80000001	; If this isn't true, we don't support 64-bit long mode, and thus should blue box of death.
	jb boot_panic

	; We now use extended CPUID to check for long mode. The earlier one just checks for if it's too old to possibly support long mode.
	mov eax, 0x80000001
	cpuid

	test edx, 1 << 29      ; Check if we do support long mode (bit 30)
	; If zero, we panic.
	je boot_panic_invalid_arch



create_page_tables:
	; We'll use names like 'p4', 'p3', etc, because it's just easier.

	; We need to link up P4 to P3.
	mov eax, p3_table
	or eax, 11b		; Present and writable.
	mov [p4_table], eax	; And write it to the P4 table.

	; Now we need to link P3 to P2.
	mov eax, p2_table	; Put the location of P2 in,
	or eax, 11b		; Present and writable flags...
	mov [p3_table], eax	; And write to the P3 table.

	
	; Now we need to work on P2.
	; P2 needs to map the first 6MB (should be all we need) with identity mapping, for now.
	; (We'll get around to moving this to the higher half later.)
	; For now, let's identity map everything we can under P2.

	; As such, we need 3 entries, and utilize a loop to do so.
	; We'll use ECX for this (we already clobbered it earlier from the checking for CPUID.)
	mov ecx, 0
	
.map_page_tables:
	; Use huge pages (2MB), map at 2MB * ecx.
	mov eax, 0x200000
	mul ecx
	or eax, 10000011b	; Present, writable, and huge.
	mov [p2_table + ecx * 8], eax	; And now write it to the table.

	inc ecx	; Increment it...
	cmp ecx, 512	; P2 needs 512 entries. (one gigabyte of identity mapping.)
	jne .map_page_tables


; OK, now we need to enable paging.
; Let's start...

enable_paging:
	lea eax, [p4_table]	; Put the address for the p4 table in EAX.
	mov cr3, eax		; Then clobber whatever is in CR3 and move it there.
	
	; We need to enable PAE. (Long mode is a superset of PAE which requires PAE to be enabled.)
	mov eax, cr4
	or eax, (1 << 5)		; Enable the PAE flag in control register 4.
	mov cr4, eax		; And put it back in CR4.

	; We assume EFER is here, because x86_64 just doesn't work without it.
	; Now, we actually enable long mode.
	mov ecx, 0xC0000080
	rdmsr			; Read model specific register (x86-64 EFER)
	or eax, (1 << 8)	; Flip bit 8.
	wrmsr			; And write back to the MSR.

	; Now we finish enable paging.
	mov eax, cr0
	or eax, (1 << 31)	; Just flip bit 31, the bit in binary is LOOONNNGGG.
	or eax, 1
	mov cr0, eax		; And write to CR0.
	

; In the future, we need to refactor the
; above code to call into these nice small utility
; functions.


; We still need to create a GDT so we can run 64-bit code.
; Let's start...
create_gdt:
	
	lgdt [gdt_64.gdtpointer]
	; Hold up just a second here. We need to pass to kern_start the multiboot header before it runs off into KiSystemStartup.
	; kern_start is multiboot-only.
	; Get around to that later.

	; Force TLB flush
	mov ecx, cr3
	mov cr3, ecx
	
	; invoke far return to go to 64-bit mode.
	push 0x08
	; do a short little dance to ensure we load the right address.
	; We do a call in a sort of odd way: return is just a funny "pop %rip".
	; at the end of the day, so take advantage of that.
	lea eax, [kern_start]
	push eax
	retf

	jmp $

BITS 64
kern_start:

	; For now, we need to terminate any other descriptors, as cs is the only one we care about.

	mov ax, 0
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov rdi, [multiboot_value]			; Give us the multiboot info we want.
	mov rsp, stack_top
	and rsp, -16	; Guarantee that we're in fact, aligned correctly.	

	extern kern_init
	call kern_init
	jmp $

global cpuid_vendor_func

cpuid_vendor_func:
; In order: RDI, RSI, RDX.
push rbp		; Don't think this is necessary, but let's do it anyway.
push rdx		; Necessary later.
mov rax, 0		; We want the vendor name.
cpuid			; The CPUID instruction we're in assembler for.
mov rax, rdx		; Move into RAX the value of RDX. Necessary later.
mov [edi], ebx		; Put into the first chunk of CPUID EBX.
mov [esi], eax		; Move into location of argument 2 (ESI) the value of EDX.
pop rdx			; RDX is one of our pointers, but is overriden by CPUID.
mov [edx], ecx		; This is the last chunk of text we need for CPUID vendor.
pop rbp			; Return the original base pointer.
mov rax, 0		; Return 0 always.
ret

global cpuid_brand_name

cpuid_brand_name:
; In order: RDI, RSI, RDX, RCX
push rbp
mov rax, [rdi]	; What part of the 3 calls we're doing do we do?


; ok, we're going to be bad and trash the r* registers real fast.

push rdx
push rcx

cpuid		; CPUID.

mov [edi], eax	; Put into the first argument the value in EAX.
mov [esi], ebx	; Put into the second argument the value in EBX.

pop rax
pop rbx

mov [ebx], ecx
mov [eax], edx

pop rbp
ret

global cpuid_family_number

cpuid_family_number:
push rbx
push rcx
push rdx
mov rax, 1
cpuid
pop rdx
pop rcx
pop rbx
ret


global get_initial_p4_table
global get_initial_p3_table
global get_initial_p2_table

get_initial_p4_table:
	mov rax, [p4_table]
	ret

get_initial_p3_table:
	mov rax, [p3_table]
	ret
	
get_initial_p2_table:
	mov rax, [p2_table]
	ret


section .rodata

; Store our multiboot pointer.
multiboot_value resd 0

gdt_64:
	dq 0	; The zero entry, which is needed for the GDT.

.code: equ $ - gdt_64
	dq (1 << 40) | (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; The GDT needs to be done like this.

.gdtpointer:
	dw $ - gdt_64 - 1
	dq gdt_64	; The pointer that the GDT wants.

section .bss
ALIGN 4096
; Note we go p2 --> p3 --> p4.
p4_table:
	resb 4096
p3_table:
	resb 4096
p2_table:
	resb 4096
ALIGN 16
stack_bottom:
	resb 16384	; Nice and big (16KiB) stack.
stack_top:

