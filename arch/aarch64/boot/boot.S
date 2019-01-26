// To keep this in the first portion of the binary.
.section ".text.boot"
 
// Make _start global.
.globl _start

_start:
	wfe
	b _start
