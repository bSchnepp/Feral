/*
Copyright (c) 2019, Brian Schnepp

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
#include <arch/x86_64/idt/idt.h>
#include <arch/x86_64/cpuio.h>
#include <arch/x86_64/cpufuncs.h>


#define X86_PIC_1_COMMAND (0x20)
#define X86_PIC_1_DATA (0x21)

#define X86_PIC_2_COMMAND (0xA0)
#define X86_PIC_2_DATA (0xA1)


static IDTDescriptor IDT[256];
static IDTLocation IDTPTR;

extern void x86_install_idt(IDTLocation *Pointer);
extern void x86_divide_by_zero(VOID);

void x86SetupIDTEntries();
INTERRUPT void DivideByZero(x86InterruptFrame *Frame);

void x86PICSendEOI(void)
{
	//x86outb(0xA0, 0x20);
	//x86outb(0x20, 0x20);
}


void x86InitializeIDT()
{
	IDTPTR.Limit = ((sizeof(IDTDescriptor)) * 256) - 1;
	UINT_PTR Location = (&IDT);
	IDTPTR.Location = Location;
	KiSetMemoryBytes(IDT, 0, (sizeof(IDTDescriptor)) * 256);
	x86_install_idt(&IDTPTR);
	/* TODO: Install routines needed. */
	KiPrintFmt("IDT Ready to work...\n");
	x86SetupIDTEntries();
	KiRestoreInterrupts(TRUE);
	/* x86_divide_by_zero();	/* Expect page fault */
}

void x86IDTSetGate(UINT8 Number, UINT_PTR Base, UINT16 Selector, UINT8 Flags)
{
	/* 0 - 255 happens to be valid, so no need for checking. */
	IDTDescriptor Descriptor;
	
	Descriptor.Offset = (UINT16)(Base & 0xFFFF);
	Descriptor.Offset2 = (UINT16)((Base >> 16) & 0xFFFF);
	/* 
		Reserved should stay reserved. (on 32-bit x86, 
		these are different fields, but the function
		is the same: don't do anything with it.
	*/
	Descriptor.RESERVED = 0;
	
	/* And the important bits. */
	Descriptor.Selector = Selector;
	Descriptor.TypeAttr = Flags;
	
#if defined(__x86_64__)
	Descriptor.Offset3 = (UINT32)((Base >> 32) & 0xFFFFFFFF);
	/* No TSS, so set to zero. */
	Descriptor.IST = 0;
#endif
	IDT[Number] = Descriptor;	
}


INTERRUPT void DivideByZero(x86InterruptFrame *Frame)
{
	KiPrintFmt("DIVIDING BY ZERO!!!\n");
	x86PICSendEOI();
}

INTERRUPT void GenericHandler(x86InterruptFrame *Frame)
{
	KiPrintFmt("Unhandled Interrupt!\n");
	x86PICSendEOI();
}


void x86SetupIDTEntries()
{
	/* 0x08 is for kernel code segment offset */
	/* 0x8E is for interrupt gate. */
	for (UINTN i = 0; i < 255; ++i)
	{
		x86IDTSetGate(i, (UINTN)(GenericHandler), 0x08, 0x8E);
	}
	x86IDTSetGate(0, (UINTN)(DivideByZero), 0x08, 0x8E);
}
