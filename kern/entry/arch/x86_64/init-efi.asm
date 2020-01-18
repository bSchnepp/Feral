KERN_VIRT_OFFSET EQU 0xFFFFFFFFC0000000


GLOBAL _efi_start
BITS 64

section .earlydata
gdt_64:
	dq 0	; The zero entry, which is needed for the GDT.

.code: equ $ - gdt_64
	dq (1 << 40) | (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53) ; The GDT needs to be done like this.

.gdtpointer:
	dw $ - gdt_64 - 1
	dq gdt_64	; The pointer that the GDT wants.

section .earlytext
global _efi_start
_efi_start:
	mov esp, stack_top - KERN_VIRT_OFFSET
	
	; Take the argument we got
	mov [efi_boot_info - KERN_VIRT_OFFSET], rdi
	
	; Before moving on, make sure the whole BSS section
	; is zeroed out.
	xor eax, eax
	mov edi, (bss_start - KERN_VIRT_OFFSET)
	mov ecx, (bss_end - KERN_VIRT_OFFSET)
	sub ecx, edi
	cld
	rep stosb


create_page_tables:	
	; Now we need to work on P2.
	; P2 needs to map the first 6MB (should be all we need) with identity mapping, for now.

	; As such, we need 3 entries, and utilize a loop to do so.
	; We'll use ECX for this (we already clobbered it earlier from the checking for CPUID.)
	xor ecx, ecx


.map_page_tables:
	; Use huge pages (2MB), map at 2MB * ecx.
	mov eax, 0x200000
	mul ecx
	or eax, 10000011b	; Present, writable, and huge.
	mov [(p2_table - KERN_VIRT_OFFSET) + ecx * 8], eax	; And now write it to the table.

	inc ecx	; Increment it...
	cmp ecx, 512	; P2 needs 512 entries. (one gigabyte of identity mapping.)
	jne .map_page_tables


; OK, now we need to enable paging.
; Let's start...

enable_paging:
	mov rax, (p4_table - KERN_VIRT_OFFSET)	; Put the address for the p4 table in EAX.
	mov cr3, rax		; Then clobber whatever is in CR3 and move it there.
	
	; We need to enable PAE. (Long mode is a superset of PAE which requires PAE to be enabled.)
	mov rax, cr4
	or rax, (1 << 5)		; Enable the PAE flag in control register 4.
	mov cr4, rax		; And put it back in CR4.

	; We assume EFER is here, because x86_64 just doesn't work without it.
	; Now, we actually enable long mode.
	mov rcx, 0xC0000080
	rdmsr			; Read model specific register (x86-64 EFER)
	or rax, (1 << 8)	; Flip bit 8.
	wrmsr			; And write back to the MSR.

	; Now we finish enable paging.
	mov rax, cr0
	or rax, (1 << 31)	; Just flip bit 31, the bit in binary is LOOONNNGGG.
	or rax, 1
	mov cr0, rax		; And write to CR0.

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
	mov rcx, cr3
	mov cr3, rcx
	
	
	
	; invoke far return to go to 64-bit mode.
	push 0x08
	; do a short little dance to ensure we load the right address.
	; We do a call in a sort of odd way: return is just a funny "pop %rip".
	; at the end of the day, so take advantage of that.
	mov rax, dword (kern_start - KERN_VIRT_OFFSET)
	push rax
	retf

	jmp $


SECTION .text
	
kern_start:
	; For now, we need to terminate any other descriptors, as cs is the only one we care about.

	mov ax, 0
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov rdi, [efi_boot_info - KERN_VIRT_OFFSET]			; Give us the multiboot info we want.
	mov rsp, qword (stack_top)
	and rsp, -16	; Guarantee that we're in fact, aligned correctly.	

	; Force TLB flush (again)
	mov rcx, cr3
	mov cr3, rcx

	extern kern_init
	mov rax, kern_init
	call rax
endloop:
	sti ; Force enable interrupts, or else bad things happen!
	hlt
	jmp endloop

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
	resb 16384	; Nice and big (16KiB) stack.
stack_top:
bss_end:

section .data
; Store our custom efi boot info pointer.
efi_boot_info dq 0

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
	
