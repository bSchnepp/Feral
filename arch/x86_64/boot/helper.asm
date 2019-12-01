BITS 64
section .text

global cpuid_vendor_func
global cpuid_brand_name
global cpuid_family_number

global x86_install_idt
global x86_divide_by_zero

global x86_interrupt_1
global x86_interrupt_2
global x86_interrupt_3
global x86_interrupt_4
global x86_interrupt_5
global x86_interrupt_6
global x86_interrupt_7
global x86_interrupt_8
global x86_interrupt_9
global x86_interrupt_10
global x86_interrupt_11
global x86_interrupt_12
global x86_interrupt_13
global x86_interrupt_14
global x86_interrupt_15
global x86_interrupt_16
global x86_interrupt_17
global x86_interrupt_18
global x86_interrupt_19

global x86_interrupt_33

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

; Debug exception
x86_interrupt_1:
int 0x01
ret

; Non-maskable interrupt
x86_interrupt_2:
int 0x02
ret

; Breakpoint
x86_interrupt_3:
int 0x03
ret

; Overflow
x86_interrupt_4:
int 0x04
ret

; Bound-range exception
x86_interrupt_5:
int 0x05
ret

; Invalid opcode
x86_interrupt_6:
int 0x06
ret

; Device Not Available
x86_interrupt_7:
int 0x07
ret

; Double Fault
x86_interrupt_8:
int 0x08
ret

; RESERVED
x86_interrupt_9:
int 0x09
ret

; Invalid TSS
x86_interrupt_10:
int 0x0A
ret

; Segment Not Present
x86_interrupt_11:
int 0x0B
ret

; Stack Exception
x86_interrupt_12:
int 0x0C
ret

; General Protection Exception
x86_interrupt_13:
int 0x0D
ret

; Page Fault
x86_interrupt_14:
int 0x0E
ret

; RESERVED
x86_interrupt_15:
int 0x0F
ret

; floating point exception
x86_interrupt_16:
int 0x10
ret

; Alignment exception
x86_interrupt_17:
int 0x11
ret

; Machine check exception
x86_interrupt_18:
int 0x12
ret

; SIMD exception
x86_interrupt_19:
int 0x13
ret


; Simulated keyboard interrupt
x86_interrupt_33:
int 33
ret
