/*
Copyright (c) 2019, 2021, Brian Schnepp

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

#include <feral/kern/frlos.h>
#include <feral/kern/krnlfuncs.h>
#include <feral/kern/krnlfirmware.h>

#include <arch/x86_64/idt/idt.h>
#include <arch/x86_64/cpuio.h>
#include <arch/x86_64/cpufuncs.h>
#include <arch/x86_64/mm/pageflags.h>

extern void x86_divide_by_zero(VOID);
extern void x86_interrupt_3(VOID);
extern void x86_interrupt_14(VOID);
extern void x86_interrupt_33(VOID);

extern VOID VgaSwapBuffers(VOID);

void x86DisablePIC(VOID);
INTERRUPT void DivideByZero(x86InterruptFrame *Frame);
INTERRUPT void GenericHandler(x86InterruptFrame *Frame);
INTERRUPT void GenericHandlerPIC1(x86InterruptFrame *Frame);
INTERRUPT void GenericHandlerPIC2(x86InterruptFrame *Frame);
INTERRUPT void DoubleFaultHandler(x86InterruptFrame *Frame);
INTERRUPT void PS2KeyboardHandler(x86InterruptFrame *Frame);
INTERRUPT void PageFaultHandler(x86InterruptFrame *Frame);
INTERRUPT void PITHandler(x86InterruptFrame *Frame);



void x86PICSendEOIPIC1(void);
void x86PICSendEOIPIC2(void);

void x86PICSendEOIPIC1(void)
{
	x86outb(X86_PIC_1_COMMAND, 0x20);
}

void x86PICSendEOIPIC2(void)
{
	x86outb(X86_PIC_2_COMMAND, 0x20);
	x86outb(X86_PIC_1_COMMAND, 0x20);
}

void x86DisablePIC(VOID)
{
	x86outb(X86_PIC_1_DATA, 0xFF);
	x86outb(X86_PIC_2_DATA, 0xFF);
}

INTERRUPT void DivideByZero(x86InterruptFrame *Frame)
{
	UNUSED(Frame);
	KiPrintFmt("DIVIDING BY ZERO!!!\n");
}

INTERRUPT void GenericHandler(x86InterruptFrame *Frame)
{
	UNUSED(Frame);
	KiPrintFmt("Unhandled Interrupt!\n");
}

INTERRUPT void GenericHandlerPIC1(x86InterruptFrame *Frame)
{
	UNUSED(Frame);
	KiPrintFmt("Unhandled Interrupt! (PIC1) \n");
	x86PICSendEOIPIC1();
}

INTERRUPT void GenericHandlerPIC2(x86InterruptFrame *Frame)
{
	UNUSED(Frame);
	KiPrintFmt("Unhandled Interrupt! (PIC2) \n");
	x86PICSendEOIPIC2();
}

INTERRUPT void DoubleFaultHandler(x86InterruptFrame *Frame)
{
	UNUSED(Frame);
	KiStopError(STATUS_ERROR);
}

INTERRUPT void PageFaultHandler(x86InterruptFrame *Frame)
{
	UNUSED(Frame);
	KiStopError(STATUS_ERROR);
}

static UINT8 ScreenTimer = 0;

INTERRUPT void PITHandler(x86InterruptFrame *Frame)
{
	UNUSED(Frame);
	/* Invoke switching of process or something. */
	if (++ScreenTimer == 16)
	{
		/* Switch banks on the framebuffer... */
		ScreenTimer = 0;
		VgaSwapBuffers();
	}
	x86PICSendEOIPIC1();
}


typedef struct PS2KeyboardContext
{
	UINT8 ShiftModifier;
	UINT8 ControlModifier;
	UINT8 AltModifier;
	UINT8 CapsLock;
} PS2KeyboardContext;

static PS2KeyboardContext KeyboardContext = {0};

CHAR ApplyShiftIfNeeded(CHAR In);

CHAR ApplyShiftIfNeeded(CHAR In)
{
	/* There's some symbols like < and ; we have to worry about later. */
	if (In < 'A' || In > 'z')
	{
		return In;
	}

	if (KeyboardContext.ShiftModifier)
	{
		return (In & ~0x20);
	}
	else
	{
		return (In | 0x20);
	}
}

#include <keymaps/charmap.inl>

static void CheckStatusCode(UINT8 In);

static void CheckStatusCode(UINT8 In)
{
	BOOL Released = (In & 0x80) != 0x00;
	UINT8 Index = In & 0x7F;
	CONST CHAR *Content = PS2Charmap[Index];

	UINT64 MaxLength = 6; /* Length of "LShift" */
	UINT64 ContentLength = 0;
	KiGetStringLength(Content, &ContentLength);

	UINT64 CompareLength
		= (ContentLength < MaxLength) ? ContentLength : MaxLength;

	BOOL IsShift = FALSE;
	KiCompareMemory(Content, "LShift", CompareLength, &IsShift);
	if (!IsShift)
	{
		/* Try again for right shift */
		KiCompareMemory(Content, "RShift", CompareLength, &IsShift);
	}

	if (!Released && IsShift)
	{
		KeyboardContext.ShiftModifier = !(KeyboardContext.CapsLock);
		return;
	}
	else if (Released && IsShift)
	{
		KeyboardContext.ShiftModifier = (KeyboardContext.CapsLock);
		return;
	}

	/* Also handle caps lock case. */
	BOOL IsCapsLock = FALSE;
	KiCompareMemory(Content, "CapsLock", CompareLength, &IsCapsLock);
	if (!Released && IsCapsLock)
	{
		/* Invert on press event. */
		KeyboardContext.ShiftModifier
			= !(KeyboardContext.ShiftModifier);
		KeyboardContext.CapsLock = !(KeyboardContext.CapsLock);
		return;
	}

	UINT64 MaxBackspaceLength = 9; /* Length of "Backspace" */
	BOOL IsBackspace = FALSE;

	CompareLength = (ContentLength < MaxBackspaceLength)
				? ContentLength
				: MaxBackspaceLength;

	KiCompareMemory(Content, "Backspace", CompareLength, &IsBackspace);
	if (!Released && IsBackspace)
	{
		KiBackspace();
		return;
	}
}

/* TODO: Support new keymaps... */
static CHAR InternalConvertPS2KeyToASCII(CHAR In);
static CHAR InternalConvertPS2KeyToASCII(CHAR In)
{
	/* TODO: convert this to a lookup table... */
	UINT64 ContentLength = 0;
	UINT8 Index = In & 0x7F;
	const CHAR *Content = PS2Charmap[Index];
	KiGetStringLength(Content, &ContentLength);

	CHAR ProperLetter = Content[0];
	if (In == 0x1C || In == 0x5A)
	{
		return '\n';
	}
	else if (In == 0x39 || In == 0x29)
	{
		return ' ';
	}

	if (ContentLength != 1)
	{
		return '\0';
	}

	return ApplyShiftIfNeeded(ProperLetter);
}


/* We'll need to pull this out into a proper driver later. */
INTERRUPT void PS2KeyboardHandler(x86InterruptFrame *Frame)
{
	UNUSED(Frame);
	CHAR Keycode;
	Keycode = x86inb(0x60); /* Data is 0x60. */
	CheckStatusCode(Keycode);
	CHAR Letter = InternalConvertPS2KeyToASCII(Keycode);
	CHAR Buffer[2] = {'\0', '\0'};
	Buffer[0] = Letter;
	Buffer[1] = '\0';
	if (Letter && (Keycode & 0x7F) == Keycode)
	{
		KiPrintFmt(Buffer);
	}
	x86PICSendEOIPIC1();
}



/* For easy reference, on the x86, an interrupt with these values
 * are for these reasons.
 *
 * 0		Divide by zero
 * 1		Debug exception
 * 2		Non-maskable interrupt
 * 3		Breakpoint
 * 4		Overflow
 * 5		Bound Range Exceeded
 * 6		Invalid Opcode
 * 7		Device Not Available
 * 8		Double Fault
 * 9		Coprocessor???
 * a		Invalid TSS
 * b		Segment Not Present
 * c		Stack Segment Fault
 * d		General Protection Fault
 * e		Page Fault
 * f		Reserved
 * 10		Floating Point Exception
 * 11		Alignment Check
 * 12		Machine Check
 * 13		SIMD Float Exception
 * 14		Virtualization Exception
 * 	Reserved
 * 1E		Security Exception
 * 1F		FPU Error???
 */

void x86SetupIDTEntries()
{
	/* In the base case, ensure everything is filled by default. */
	for (UINTN i = 0; i < 255; ++i)
	{
		x86IDTSetGate(i, (UINT_PTR)(GenericHandler), 0x08, 0x8E);
	}
	for (UINTN i = 0x28; i < 0x30; ++i)
	{
		x86IDTSetGate(i, (UINT_PTR)(GenericHandlerPIC2), 0x08, 0x8E);
	}

	/* 0x08 is for kernel code segment offset */
	/* 0x8E is for interrupt gate. */

	/* On number 14 (page fault), install custom handler. */
	x86IDTSetGate(0x00, (UINT_PTR)(DivideByZero), 0x08, 0x8E);
	x86IDTSetGate(0x08, (UINT_PTR)(DoubleFaultHandler), 0x08, 0x8E);
	x86IDTSetGate(0xe, (UINT_PTR)(PageFaultHandler), 0x08, 0x8E);

	x86IDTSetGate(0x20, (UINT_PTR)(PITHandler), 0x08, 0x8E);
	x86IDTSetGate(0x21, (UINT_PTR)(PS2KeyboardHandler), 0x08, 0x8E);
	x86IDTSetGate(0x2C, (UINT_PTR)(PS2KeyboardHandler), 0x08, 0x8E);
}

VOID KiRestoreInterrupts(BOOLEAN value)
{
	if (value)
	{
		__asm__ __volatile__("sti");
	}
	else
	{
		__asm__ __volatile__("cli");
	}
}
