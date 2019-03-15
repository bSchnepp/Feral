[global x86_read_cr0]
[global x86_write_cr0]
[global x86_read_cr3]
[global x86_write_cr3]

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
