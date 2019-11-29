[global x86_read_cr0]
[global x86_write_cr0]
[global x86_read_cr3]
[global x86_write_cr3]
[global x86_io_stall]

x86_read_cr0:
	mov rax, cr0
	ret

x86_write_cr0:
	mov cr0, rdi
	ret

x86_read_cr3:
	mov rax, cr3
	ret

x86_write_cr3:
	mov cr3, rdi
	ret
	
x86_io_stall:
	; do what that other kernel does I guess
	mov rax, rdi
	out 0x80, al
	ret
