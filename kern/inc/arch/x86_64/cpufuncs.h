/*
Copyright (c) 2018, 2021, Brian Schnepp

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute, execute,
and transmit the Software, and to prepare derivative works of the Software,
and to permit third-parties to whom the Software is furnished to do so, all
subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer, must
be included in all copies of the Software, in whole or in part, and all
derivative works of the Software, unless such copies or derivative works are
solely in the form of machine-executable object code generated by a source
language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY
DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
 */

#ifndef _FERAL_ARCH_x86_64_CPUFUNCS_H_
#define _FERAL_ARCH_x86_64_CPUFUNCS_H_

#include <feral/stdtypes.h>

void cpuid_vendor_func(DWORD* part1, DWORD* part2, DWORD* part3);
void cpuid_brand_name(DWORD* part1, DWORD* part2, DWORD* part3,
	DWORD* part4); /* See comment in kernel_main.c for usage. Cleanup later.
			*/
UINT32 cpuid_family_number(void);

UINT64 x86_read_cr0(void);
UINT64 x86_read_cr3(void);

VOID x86_write_cr0(UINT64 val);
VOID x86_write_cr3(UINT64 val);

VOID x86_io_stall(VOID);

#define INTERRUPT __attribute__((interrupt))

typedef struct x86InterruptFrame
{
	UINTN IP;
	UINTN CS;
	UINTN FLAGS;
	UINTN SP;
	UINTN SS;
} x86InterruptFrame;

/* Useful ("stable") functions. Refactor elsewhere later. */
VOID KiRestoreInterrupts(IN BOOLEAN value);



#endif
