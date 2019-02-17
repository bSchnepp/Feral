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


// Define internal kernel functions here. (hence 'krnlfun'.)

#include <feral/string.h>
#include <feral/stdtypes.h>
#include <feral/feralstatus.h>
#include <feral/kern/krnlfuncs.h>
#include <feral/kern/frlos.h>

/* 
	Define internal kernel functions here. (hence 'krnlfun'.) 
 */

#include <stdarg.h>

#if defined(__x86_64__)
#include <arch/x86_64/vga/vga.h>


FERALSTATUS KiPutChar(CHAR c)
{
	VgaPutChar(c);
	return STATUS_SUCCESS;
}

#define PRINT_LINE_GENERIC()												\
	UINT64 length;															\
	FERALSTATUS feralStatusError = KiGetStringLength(string, &length);		\
	if (!(feralStatusError == STATUS_SUCCESS))								\
	{																		\
		return feralStatusError;												\
	}


// Start, end, size.
FERALSTATUS KiCopyMemory(IN VOID* Source, IN VOID* Dest, IN UINT64 Amount)
{
	if ((Source == NULL) || (Dest == NULL))
	{
		return STATUS_INVALID_MEMORY_LOCATION;
	}
	CHAR* srcAsChar = (CHAR*)(Source);
	CHAR* dstAsChar = (CHAR*)(Dest);
	for (UINT64 c = 0; c < Amount; ++c)
	{
		*dstAsChar++ = *srcAsChar++;
	}
	return STATUS_SUCCESS;
}

//First, Second, Size, Equal
FERALSTATUS KiCompareMemory(IN VOID* Source, IN VOID* Dest, IN UINT64 Amount, OUT BOOL* Val)
{
	if ((Source == NULL) || (Dest == NULL))
	{
		return STATUS_INVALID_MEMORY_LOCATION;
	}
	for (UINT64 i = 0; i < Amount; i++)
	{
		// Assume they are UINTN. This will probably need to be changed.
		UINTN* Src = (Source + i);
		UINTN* Dst = (Dest + i);
		if (!(*Src == *Dst))
		{
			*(Val) = 0;
			break;
		}
		*(Val) = 1;
	}
	return STATUS_SUCCESS;
}

//TODO: Implement the rest of this stuff.
#if 0
//Start, New location, size
FERALSTATUS KiMoveMemory(IN VOID*, IN CONST VOID*, IN UINTN);

//Where, with what, and how many UINTNs to set.
FERALSTATUS KiSetMemory(INOUT VOID*, IN UINTN, IN UINTN);

// Same as above, but with bytes.
FERALSTATUS KiSetMemoryBytes(INOUT VOID*, IN UINT8, IN UINTN);
#endif


FERALSTATUS KiGetStringLength(IN STRING String, OUT UINT64* Length)
{
	if (String == NULL)
	{
		return STATUS_INVALID_MEMORY_LOCATION;
	}
	UINT64 Len = 0;
	while (String[Len])	// false is just 0, so when we dereference and find equal to 0, we terminate.
	{
		Len++;
	}
	*Length = Len;
	return STATUS_SUCCESS;
}

/*
//Same as above but with a wide string.
FERALSTATUS KiGetWideStringLength(IN WSTRING, OUT* UINTN);
*/

/* Todo: aarch64 version(s). This breaks aarch64 port.*/

FERALSTATUS KiPrintLine(IN STRING string)
{
	PRINT_LINE_GENERIC();
	// Ok, we call VgaPrintln and use a black on white color set.
	VgaPrintln(VGA_WHITE, VGA_BLACK, string, length);
	return STATUS_SUCCESS;
}

FERALSTATUS KiPrintGreyLine(STRING string)
{
	PRINT_LINE_GENERIC();
	VgaPrintln(VGA_LIGHT_GREY, VGA_BLACK, string, length);
	return STATUS_SUCCESS;
}

FERALSTATUS KiPrintWarnLine(STRING string)
{
	PRINT_LINE_GENERIC();
	VgaPrintln(VGA_LIGHT_BROWN, VGA_BLACK, string, length);
	return STATUS_SUCCESS;
}

FERALSTATUS KiPrint(STRING string)
{
	PRINT_LINE_GENERIC();
	for (UINT64 i = 0; i < length; ++i)
	{
		CHAR let = string[i];
		KiPutChar(let);
	}
	return STATUS_SUCCESS;
}



/* Internal function: suppress warning (for now) */
VOID internalItoa(UINT64 val, STRING buf);
VOID internalSignedItoa(INT64 val, STRING buf);
VOID internalItoaBaseChange(UINT64 val, STRING buf, UINT8 radix);


VOID internalItoaBaseChange(UINT64 val, STRING buf, UINT8 radix)
{
	UINT64 len = 0;

	if (val == 0)
	{
		*buf = '0';
		*(buf + 1) = '\0';
		return;
	}
	
	for (UINT64 valCopy = val; valCopy != 0; valCopy /= radix)
	{
		CHAR rem = valCopy % radix;
		if (rem <= 9 && rem >= 0)
		{
			buf[len++]  =  rem + '0';
		} else if (rem < 35)
		{
			buf[len++]  =  (rem - 10) + 'a';
		} else {
			buf[len++]  =  (rem - 36) + 'A';
		}
	}
	
	for (UINT64 i = 0; i < len / 2; ++i)
	{
		CHAR tmp = buf[i];
		buf[i] = buf[len - i  - 1];
		buf[len - i  - 1] = tmp;
	}
	
	/* Terminate the string. */
	buf[len] =  '\0';
}

/* 
	We don't have a kmalloc() yet, so we can't just free and alloc whenever we want.
	Thus, our internal itoa _must_ be in-place.
	
	buf must be at least size 2, and is at most going to use 21 characters.
*/
VOID internalItoa(UINT64 val, STRING buf)
{
	internalItoaBaseChange(val, buf, 10);
}



/* 
	We don't have a kmalloc() yet, so we can't just free and alloc whenever we want.
	Thus, our internal itoa _must_ be in-place.
	
	buf must be at least size 2, and is at most going to use 22 characters.
*/
VOID internalSignedItoa(INT64 val, STRING buf)
{
	UINT64 len = 0;
	UINT64 base = 10;
	
	if (val >= 0)
	{
		return internalItoa(val, buf);
	}
	
	/* We can assume we're negative: now we can avoid doing / or % on a negative value. */
	for (INT64 overzero = 0 - val; overzero != 0; overzero /= base)
	{
		CHAR rem = overzero % base;
		buf[len++]  =  rem + '0';
	}
	
	/* Append the -. */
	buf[len++] = '-';
	
	for (UINT64 i = 0; i < len / 2; ++i)
	{
		CHAR tmp = buf[i];
		buf[i] = buf[len - i  - 1];
		buf[len - i  - 1] = tmp;
	}
	
	/* Terminate the string. */
	buf[len] =  '\0';
}

FERALSTATUS KiPrintFmt(const STRING fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	BOOL upState = FALSE;
	UINT8 repeatedCount = 0;
	
	UINT64 index = 0;
	for (CHAR cur = fmt[0]; cur != '\0'; cur = fmt[++index])
	{	
		if (cur == '%' && !upState)
		{
			/* Push up */
			upState = TRUE;
		} else if (upState)
		{
			if (cur == 's')
			{
				STRING valistnext;
				valistnext = va_arg(args, STRING);
				KiPrint(valistnext);
			} else if (cur == 'i' || cur == 'l') {
				INT32 valistnext;
				valistnext = va_arg(args, INT32);
				/* Create a buffer to store in. Integer is never longer than 20, so... */
				CHAR buf[21] = {0};
				internalSignedItoa((UINT64)valistnext, buf);
				KiPrint(buf);
			} else if (cur == 'l') {
				INT64 valistnext;
				valistnext = va_arg(args, INT64);
				/* Create a buffer to store in. Integer is never longer than 20, so... */
				CHAR buf[21] = {0};
				internalSignedItoa((UINT64)valistnext, buf);
				KiPrint(buf);
			} else if (cur == 'u') {
				UINT64 valistnext;
				valistnext = va_arg(args, UINT64);
				/* Create a buffer to store in. Integer is never longer than 21, so... */
				CHAR buf[22] = {0};
				internalItoa(valistnext, buf);
				KiPrint(buf);
			} else if (cur == 'x') {
				UINT64 valistnext;
				valistnext = va_arg(args, UINT64);
				/* Create a buffer to store in. Integer is never longer than 16, so... */
				CHAR buf[17] = {0};
				internalItoaBaseChange(valistnext, buf, 16);
				KiPrint(buf);
			} else if (cur == '%') {
				KiPrint("%");
			} else if(cur >= '0' && cur <= '9')
			{
				if (fmt[++index] != '\0')
				{
					/*  FIXME: fully support it (we only lead with 0-9, and don't count places.) */
					/* What are we repeating? */
					repeatedCount = (UINT8)(fmt[index] - '0');
					/* Go through it some times. */
					for (UINT8 c = 0; c < repeatedCount; c++)
					{
						VgaPutChar(cur);
					}
				}
			}
			/* What do you mean %llu is a thing? */
			upState = FALSE;
		} else if (cur >= ' ') {	/* bandage away bug for now. */
			VgaPutChar(cur);
		} else if (cur == '\n') {
			KiPrintLine("");
		} else if (cur == '\t') {
			/* Add 8 spaces. */
			for (int i = 0; i < 8; i++)
			{
				KiPrint(" ");
			}
		}
		

	}
	va_end(args);
	return STATUS_SUCCESS;
}

#else

// TODO
#define KiPrintWarnLine(a)
#define KiPrintGreyLine(a)
#define KiPrintLine(a)

#endif
// TODO: implement for other platforms.







FERALSTATUS FrlCreateString(IN FERALSTRING* StringLocation, UINT64 Length, WSTRING Content)
{
	StringLocation->Length = Length;
	StringLocation->Content = Content;
	return STATUS_SUCCESS;
}

FERALSTATUS FrlDeleteString(IN FERALSTRING* String)
{
	//TODO
	return STATUS_SUCCESS;
}

FERALSTATUS FrlCloneString(IN FERALSTRING* Source, IN FERALSTRING* OutLocation)
{
	//TODO
	return STATUS_SUCCESS;
}


