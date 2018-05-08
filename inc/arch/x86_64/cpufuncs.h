/*
Copyright (c) 2018, Brian Schnepp

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

typedef enum
{
	CPUID_FEATURES_ECX_SSE3       = (1 << 0),
	CPUID_FEATURES_ECX_PCLMUL     = (1 << 1),
	CPUID_FEATURES_ECX_DTES64     = (1 << 2),
	CPUID_FEATURES_ECX_MONITOR    = (1 << 3),
	CPUID_FEATURES_ECX_DS_CPL     = (1 << 4),
	CPUID_FEATURES_ECX_VMX        = (1 << 5),
	CPUID_FEATURES_ECX_SMX        = (1 << 6),
	CPUID_FEATURES_ECX_EST        = (1 << 7),
	CPUID_FEATURES_ECX_TM2        = (1 << 8),
	CPUID_FEATURES_ECX_SSSE3      = (1 << 9),
	CPUID_FEATURES_ECX_CID        = (1 << 10),
	// osdev wiki says there is no 11. OK. Just going to trust that for now.
	CPUID_FEATURES_ECX_FMA        = (1 << 12),
	CPUID_FEATURES_ECX_CX16       = (1 << 13),
	CPUID_FEATURES_ECX_ETPRD      = (1 << 14),
	CPUID_FEATURES_ECX_PDCM       = (1 << 15),
}CPUID_FEATURES;	//TODO...

void cpuid_vendor_func(DWORD* part1, DWORD* part2, DWORD* part3);
void cpuid_brand_name(DWORD* part1, DWORD* part2, DWORD* part3, DWORD* part4);	// See comment in kernel_main.c for usage. Cleanup later.

/* Useful ("stable") functions. Refactor elsewhere later. */
VOID KiRestoreInterrupts(IN BOOLEAN value);



#endif
