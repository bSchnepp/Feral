BITS 64
section .text

global cpuid_vendor_func
global cpuid_brand_name
global cpuid_family_number

global x86_install_idt
global x86_divide_by_zero

cpuid_vendor_func:
; In order: RDI, RSI, RDX.
push rbp		; Don't think this is necessary, but let's do it anyway.
push rdx		; Necessary later.
mov rax, 0		; We want the vendor name.
cpuid			; The CPUID instruction we're in assembler for.
mov rax, rdx		; Move into RAX the value of RDX. Necessary later.
mov [rdi], ebx		; Put into the first chunk of CPUID EBX.
mov [rsi], eax		; Move into location of argument 2 (ESI) the value of EDX.
pop rdx			; RDX is one of our pointers, but is overriden by CPUID.
mov [rdx], ecx		; This is the last chunk of text we need for CPUID vendor.
pop rbp			; Return the original base pointer.
mov rax, 0		; Return 0 always.
ret

cpuid_brand_name:
; In order: RDI, RSI, RDX, RCX
push rbp
mov rax, [rdi]	; What part of the 3 calls we're doing do we do?


; ok, we're going to be bad and trash the r* registers real fast.

push rdx
push rcx

cpuid		; CPUID.

mov [rdi], eax	; Put into the first argument the value in EAX.
mov [rsi], ebx	; Put into the second argument the value in EBX.

pop rax
pop rbx

mov [rbx], ecx
mov [rax], edx

pop rbp
ret


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

; RDI is the parameter.
x86_install_idt:
lidt [rdi]
ret


; We're free to taint rax and rcx.
; This is done to test the IDT got set up right.
x86_divide_by_zero:
mov rax, 1
mov rcx, 0
mov rdx, 0
div rcx
ret

