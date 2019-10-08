/*
Copyright (c) 2018, 2019, Brian Schnepp

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


#include <feral/stdtypes.h>

#ifndef _FERAL_X86_IDT_H_
#define _FERAL_X86_IDT_H_

/* 
	In the future, if the part being
	interrupted was a *user* process, we need
	to defer the interrupt to it's appropriate runtime driver.
 */
#if defined(__x86_64__)
typedef struct
{
	UINT16 Offset;	 /* First 16 bits of the address (0 - 15) */
	UINT16 Selector;
	UINT8 IST;
	UINT8 TypeAttr;
	UINT16 Offset2;	 /* Second 16 bits (16 - 31) */
	UINT32 Offset3;	 /* Last 4 bytes of the address (32 - 61) */
	UINT32 RESERVED; 	/* These are reserved. */
}IDTDescriptor PACKED;
#else
typedef struct
{
	UINT16 Offset;	/* First 16 bits of the address */
	UINT16 Selector;
	UINT8 RESERVED;
	UINT8 TypeAttr;
	UINT16 Offset2;	/* Second 16 bits */
}IDTDescriptor PACKED;
#endif

typedef struct
{
	UINT16 Limit;
	UINT_PTR Location;
}IDTLocation PACKED;


void x86InitializeIDT(VOID);

void x86IDTSetGate(IN UINT8 Number, IN UINT_PTR Base, 
	IN UINT16 Selector, IN UINT8 Flags);

#endif
