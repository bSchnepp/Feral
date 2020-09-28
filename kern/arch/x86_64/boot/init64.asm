BITS 64

; FFFFFFFFC0000000, which is the full P4, P3, but entry 0 for P2.
KERN_VIRT_OFFSET EQU 0xFFFFFF8000000000

GLOBAL _start64

; This is the blue box of death. We'll worry about a useful panic() later.
; The real panic() (KeInternalWarn() for warning, KeStopError() for actual 'panic()') later.
boot_panic:
	cli
	hlt
	jmp $
	
boot_panic_invalid_arch:
	cli
	hlt
	jmp $
	

section .earlydata
gdt_64:
.null: equ $ - gdt_64
	dq 0	; The zero entry, which is needed for the GDT.

.code: equ $ - gdt_64
	dq (1 << 40) | (1 << 43) | (1 << 44) | (1 << 47) ; The GDT needs to be done like this.

.data: equ $ - gdt_64
	dq (1 << 40) | (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; The GDT needs to be done like this.

.gdtpointer:
	dw $ - gdt_64 - 1
	dq gdt_64	; The pointer that the GDT wants.

section .earlytext
_start64:
	mov rsp, stack_top - KERN_VIRT_OFFSET

	; Push ebx for when we need it later. (holds multiboot struct)
	mov [efi_value - KERN_VIRT_OFFSET], rdi

	; Before moving on, make sure the whole BSS section
	; is zeroed out.
	xor rax, rax
	mov rdi, (bss_start - KERN_VIRT_OFFSET)
	mov rcx, (bss_end - KERN_VIRT_OFFSET)
	sub rcx, rdi
	cld
	rep stosb
	
.map_page_tables:
	; Use huge pages (2MB), map at 2MB * ecx.
	mov rax, 0x200000
	mul rcx
	or rax, 10000011b	; Present, writable, and huge.
	mov [(p2_table - KERN_VIRT_OFFSET) + rcx * 8], rax	; And now write it to the table.

	inc rcx	; Increment it...
	cmp rcx, 512	; P2 needs 512 entries. (one gigabyte of identity mapping.)
	jne .map_page_tables

gdt_setup:
	lgdt [gdt_64.gdtpointer]
	
	; Unlike in MB2 mode, there isn't really
	; a reaosn to enable paging, since UEFI does that for us.
	; Instead, just set up our page tables here.
	mov rax, (p4_table - KERN_VIRT_OFFSET)	; Put the address for the p4 table in EAX.
	mov cr3, rax		; Then clobber whatever is in CR3 and move it there.

	push 0x08

	mov rax, qword (kern_start - 0)
	jmp rax
	jmp $


SECTION .text
BITS 64
	
kern_start:
	; For now, we need to terminate any other descriptors, as cs is the only one we care about.
	mov ax, 0
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov rdi, [efi_value - KERN_VIRT_OFFSET]			; Give us the multiboot info we want.
	mov rsp, qword (stack_top)
	and rsp, -16	; Guarantee that we're in fact, aligned correctly.	

	; Force TLB flush (again). Just be extra safe.
	mov rcx, cr3
	mov cr3, rcx

	; Ensure we still get out GDT in higher half
	lea rax, [gdt_64.gdtpointer]
	add rax, KERN_VIRT_OFFSET
	jmp $
	lgdt [rax]

	extern kern_init
	mov rax, kern_init
	call rax
endloop:
	sti ; Force enable interrupts, or else bad things happen!
	hlt
	jmp endloop

global p4_table
global p3_table
global p2_table

global get_initial_p4_table
global get_initial_p3_table
global get_initial_p2_table

get_initial_p4_table:
	mov rax, p4_table
	ret

get_initial_p3_table:
	mov rax, p3_table
	ret
	
get_initial_p2_table:
	mov rax, p2_table
	ret


section .bss
bss_start:

; Stack overflow will cause a problem in the p4 table.
; FIXME

ALIGN 16
stack_bottom:
	resb 65536	; Nice and big (64KiB) stack.
stack_top:
bss_end:

section .data
; Store our multiboot pointer.
efi_value dd 0

ALIGN 4096
p4_table:
	dq (p3_table - KERN_VIRT_OFFSET) + 3
	times 510 dq 0
	dq (p3_table - KERN_VIRT_OFFSET) + 3
	
p3_table:
	dq (p2_table - KERN_VIRT_OFFSET) + 3
	times 510 dq 0
	dq (p2_table - KERN_VIRT_OFFSET) + 3

p2_table:
	times 512 dq 0	; Declare 512 entries.
	
