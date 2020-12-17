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

#include <stdint.h>

#include <feral/feralstatus.h>
#include <feral/stdtypes.h>
#include <feral/kern/frlos.h>

#include <arch/x86_64/vga/vga.h>
#include <arch/x86_64/vga/vgaregs.h>
#include <arch/x86_64/cpuio.h>

#include <arch/x86_64/mm/pageflags.h>

static VgaContext CurGraphicsContext = {0};
static UINT16 OtherBuffer[80 * 25];
static VgaContext *CurVgaContext;


typedef UINT16 ColorValue;


/* Internal function, so suppress warning. */
VOID internalVgaPushUp(VOID);
UINT16 internalGetItem(UINT16 Row, UINT16 Col);
VOID internalSetItem(UINT16 Row, UINT16 Col, UINT16 content);

UINT16 internalGetItem(UINT16 Row, UINT16 Col)
{
	return CurVgaContext
		->SwappedBuffer[(Row * CurVgaContext->ScreenWidth) + Col];
}

VOID internalSetItem(UINT16 Row, UINT16 Col, UINT16 content)
{
	CurVgaContext->SwappedBuffer[(Row * CurVgaContext->ScreenWidth) + Col]
		= content;
}

VOID internalVgaPushUp(VOID)
{
	UINT16 Row = 0;
	UINT16 RowSize = 2 * (CurVgaContext->ScreenWidth);
	char *Src = CurVgaContext->SwappedBuffer + RowSize;
	char *Dst = CurVgaContext->SwappedBuffer + 0;
	char *LastRow = CurVgaContext->SwappedBuffer + ((CurVgaContext->ScreenHeight - 1) * RowSize);
	UINT64 Amt = 2 * (CurVgaContext->ScreenWidth) * (CurVgaContext->ScreenHeight - 1);
	KiCopyMemory(Dst, Src, Amt);
	KiSetMemoryBytes(LastRow, 0, RowSize);
	VgaMoveCursor(0, CurVgaContext->ScreenHeight - 1);
}


/**
	Prepares a VGA context for use by the kernel in
	pre-initialization stages.
 */
UINT8 VgaPrepareEnvironment(VOID *Framebuffer, UINT16 FramebufferBPP, UINT32 FramebufferWidth, UINT32 FramebufferHeight, BOOL FramebufferTextOnly)
{
	// Ensure a bit in port 0x03C2 is set.
	UINT8 miscreg = x86inb(0x3CC);
	x86outb(VGA_MISC_OUTPUT_REG, (miscreg | 0xE7));

	CurVgaContext = &(CurGraphicsContext);
	CurVgaContext->BPP = FramebufferBPP;
	CurVgaContext->Framebuffer = (UINT8 *)(KERN_PHYS_TO_VIRT(Framebuffer));
	CurVgaContext->ScreenWidth = FramebufferWidth;
	CurVgaContext->ScreenHeight = FramebufferHeight;

	CurVgaContext->Background = VGA_BLACK;
	CurVgaContext->Foreground = VGA_WHITE;
	CurVgaContext->Highlight = VGA_LIGHT_BROWN;

	CurVgaContext->TextMode = FramebufferTextOnly;
	CurVgaContext->FollowingInput = 0;

	CurVgaContext->CurrentRow = 0;
	CurVgaContext->CurrentCol = 0;

	/* Until we can allocate host memory for this, don't use double buffering anymore. */
	//CurVgaContext->SwappedBuffer = OtherBuffer;
	CurVgaContext->SwappedBuffer = CurVgaContext->Framebuffer;

	return miscreg;
}

/**
	Moves the VGA cursor to a new location.
 */
VOID VgaMoveCursor(DWORD PosX, DWORD PosY)
{
	if (!CurVgaContext->CursorEnabled)
	{
		return;
	}
	UINT16 FinalPos
		= (UINT16)((PosY * (CurVgaContext->ScreenWidth)) + PosX);
	x86outb(VGA_FB_COMMAND_PORT, VGA_LOW_BYTE_COMMAND);
	x86outb(VGA_FB_DATA_PORT, (UINT8)((FinalPos) & (0x00FF)));

	x86outb(VGA_FB_COMMAND_PORT, VGA_HIGH_BYTE_COMMAND);
	x86outb(VGA_FB_DATA_PORT, (UINT8)((FinalPos >> 8) & (0x00FF)));
	VgaEntry(CurVgaContext->Highlight, CurVgaContext->Background, '\0',
		PosX, PosY);
}

VOID VgaSwapBuffers(VOID)
{
	UINT64 ScreenSize
		= (CurVgaContext->ScreenWidth * CurVgaContext->ScreenHeight);

	/* VGA is based on 16-bit values. Copy them 64 at a time. */
	for (UINT64 Offset = 0; Offset < (ScreenSize >> 2); ++Offset)
	{
		UINT64 *FramebufferAsSixtyFour
			= (UINT64 *)(CurVgaContext->Framebuffer);
		UINT64 *SwapBufferAsSixtyFour
			= (UINT64 *)(CurVgaContext->SwappedBuffer);

		FramebufferAsSixtyFour[Offset] = SwapBufferAsSixtyFour[Offset];
	}
}

/**
	Sets the state of the VGA cursor.
 */
VOID VgaSetCursorEnabled(BOOL value)
{
	if (value)
	{
		x86outb(VGA_FB_COMMAND_PORT, 0x0A);
		x86outb(VGA_FB_DATA_PORT,
			(x86inb(VGA_FB_DATA_PORT) & 0xC0)
				| 0x00); /* Read in a spot, and write a specific
					    value so that we get a nice cursor.
					  */
		x86_io_stall();
		x86outb(VGA_FB_COMMAND_PORT, 0x0B);
		x86outb(VGA_FB_DATA_PORT, (x86inb(0x3E0) & 0xE0) | 0x0F);
		x86_io_stall();
	}
	else
	{
		x86outb(VGA_FB_COMMAND_PORT, 0x0A);
		x86outb(VGA_FB_DATA_PORT, 0x20);
		x86_io_stall();
	}
	CurVgaContext->CursorEnabled = value;
}

/**
	Sets the context's "trace characters" value to true.
	This means that when keyboard input is done,
	the cursor is moved to the location of the last character added.
 */
VOID VgaTraceCharacters(BOOL value)
{
	CurVgaContext->FollowingInput = value;
}

/**
	Sets a given position on the screen with a given value.
 */
VOID VgaEntry(VgaColorValue foreground, VgaColorValue background, CHAR Letter,
	DWORD PosX, DWORD PosY)
{
	if (PosY >= CurVgaContext->ScreenHeight)
	{
		internalVgaPushUp();
	}
	ColorValue Color = (foreground << 0) | (background << 4);
	UINT16 Offset = 2 * (PosX + (PosY * CurVgaContext->ScreenWidth));
	CurVgaContext->SwappedBuffer[Offset + 0] = Letter;
	CurVgaContext->SwappedBuffer[Offset + 1] = Color;
	CurVgaContext->CurrentCol = PosX;
}

/**
	Clears the current framebuffer, and ensures
	that the whole screen, from (0, 0) to (width, height),
	is replaced with a square of the Color Color.
 */
VOID KiBlankVgaScreen(DWORD Height, DWORD Width, DWORD Color)
{
	CurVgaContext->Background = Color;
	for (UINT16 H = 0; H < Height; ++H)
	{
		for (UINT16 W = 0; W < Width; ++W)
		{
			VgaEntry(Color, Color, (CHAR)('\0'), W, H);
		}
	}
	VgaSetCurrentPosition(0, 0);
}

VOID VgaSetCurrentPosition(UINT16 X, UINT16 Y)
{
	CurVgaContext->CurrentRow = Y;
	CurVgaContext->CurrentCol = X;
}

VOID VgaGetCurrentPosition(UINT16 *X, UINT16 *Y)
{
	(*Y) = CurVgaContext->CurrentRow;
	(*X) = CurVgaContext->CurrentCol;
}

VOID VgaGetFramebufferDimensions(UINT16 *Width, UINT16 *Height)
{
	(*Width) = CurVgaContext->ScreenWidth;
	(*Height) = CurVgaContext->ScreenHeight;
}

/**
	Puts a character in the next available cell, with the
	foreground and background specified.
 */
VOID VgaAutoEntry(
	VgaColorValue foreground, VgaColorValue background, CHAR letter)
{
	/* Check if we're overflowing. */
	if (CurVgaContext->CurrentCol >= CurVgaContext->ScreenWidth)
	{
		CurVgaContext->CurrentCol = 0;
		CurVgaContext->CurrentRow++;
	}

	/* Now check if Row count is too large, in which case, move everything
	 * up one. */
	if (CurVgaContext->CurrentRow >= CurVgaContext->ScreenHeight)
	{
		CurVgaContext->CurrentRow = (CurVgaContext->ScreenHeight - 1);
		internalVgaPushUp();
	}

	/* Now in safe-to-write state. */
	VgaEntry(foreground, background, letter, CurVgaContext->CurrentCol,
		CurVgaContext->CurrentRow);
	CurVgaContext->CurrentCol++;
}

/**
	Puts a character in with the current Colors, as
	defined by the VGA context.
 */
VOID VgaPutChar(CHAR letter)
{
	VgaAutoEntry(
		CurVgaContext->Foreground, CurVgaContext->Background, letter);
	if (CurVgaContext->FollowingInput)
	{
		if (CurVgaContext->CurrentCol >= CurVgaContext->ScreenWidth)
		{
			CurVgaContext->CurrentCol = 0;
			CurVgaContext->CurrentRow++;
		}

		if (CurVgaContext->CurrentRow >= CurVgaContext->ScreenHeight)
		{
			CurVgaContext->CurrentRow
				= CurVgaContext->ScreenHeight - 1;
			internalVgaPushUp();
		}
		VgaMoveCursor(
			CurVgaContext->CurrentCol, CurVgaContext->CurrentRow);
	}
}

VOID VgaStringEntry(VgaColorValue foreground, VgaColorValue background,
	CHAR *string, DWORD length, DWORD posx, DWORD posy)
{
	UINT64 index = 0;

	for (CHAR c = *string; index < length; c = string[++index])
	{
		if (posx >= CurVgaContext->ScreenWidth)
		{
			posx = 0;
			CurVgaContext->CurrentRow = ++posy;
		}

		if (posy >= CurVgaContext->ScreenHeight)
		{
			posy = CurVgaContext->ScreenHeight - 1;
			internalVgaPushUp();
		}

		if (c == '\t')
		{
			/* Add 8 spaces. */
			for (int i = 0; i < 8; i++)
			{
				VgaPutChar(' ');
			}
		}
		else if (c == '\n')
		{
			/* Now, reset the last Row's state. */
			CurVgaContext->CurrentCol = 0;
			CurVgaContext->CurrentRow++;
		}
		else if (c == '\0')
		{
			break;
		}
		else
		{
			VgaEntry(foreground, background, c, posx++, posy);
		}
	}
}

VOID VgaPrintln(VgaColorValue foreground, VgaColorValue background,
	CHAR *string, DWORD length)
{
	VgaStringEntry(foreground, background, string, length,
		CurVgaContext->CurrentCol, CurVgaContext->CurrentRow);
	CurVgaContext->CurrentRow++;
	CurVgaContext->CurrentCol = 0;

	if (CurVgaContext->CurrentRow >= CurVgaContext->ScreenHeight)
	{
		CurVgaContext->CurrentRow = CurVgaContext->ScreenHeight - 1;
		internalVgaPushUp();
	}
	VgaMoveCursor(CurVgaContext->CurrentCol, CurVgaContext->CurrentRow);
}

VOID VgaAutoPrintln(
	VgaColorValue Foreground, VgaColorValue Background, CHAR *String)
{
	UINT64 Length = 0;
	KiGetStringLength(String, &Length);
	VgaPrintln(Foreground, Background, String, Length);
}

VOID VgaAutoPrint(
	VgaColorValue Foreground, VgaColorValue Background, CHAR *String)
{
	UINT64 Length = 0;
	KiGetStringLength(String, &Length);

	VgaStringEntry(Foreground, Background, String, Length,
		CurVgaContext->CurrentCol, CurVgaContext->CurrentRow);
	CurVgaContext->CurrentCol++;
	VgaMoveCursor(CurVgaContext->CurrentCol, CurVgaContext->CurrentRow);
}
