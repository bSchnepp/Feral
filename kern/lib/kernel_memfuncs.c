/*
Copyright (c) 2020, Brian Schnepp

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
#include <feral/feralstatus.h>

#include <feral/kern/krnlfuncs.h>

FERALSTATUS KiCopyMemory(IN VOID* Source, IN VOID* Dest, IN UINT64 Amount)
{
	if ((Source == NULL) || (Dest == NULL))
	{
		return STATUS_INVALID_MEMORY_LOCATION;
	}
	CHAR* srcAsChar = (CHAR*)(Source);
	CHAR* dstAsChar = (CHAR*)(Dest);

	/* Fastest possible non-FPU memcpy off the top of my head. */
	UINT32 Index = 0;
	for (Index = 0; Index < Amount; Index += 4)
	{
		dstAsChar[Index + 0] = srcAsChar[Index + 0];
		dstAsChar[Index + 1] = srcAsChar[Index + 1];
		dstAsChar[Index + 2] = srcAsChar[Index + 2];
		dstAsChar[Index + 3] = srcAsChar[Index + 3];
	}

	for (; Index < Amount; ++Index)
	{
		dstAsChar[Index] = srcAsChar[Index];
	}

	return STATUS_SUCCESS;
}

FERALSTATUS KiCompareMemory(
	IN VOID* Source, IN VOID* Dest, IN UINT64 Amount, OUT BOOL* Val)
{
	if ((Source == NULL) || (Dest == NULL))
	{
		return STATUS_INVALID_MEMORY_LOCATION;
	}

	CHAR* srcAsChar = (CHAR*)(Source);
	CHAR* dstAsChar = (CHAR*)(Dest);

	BOOL Comparison1 = TRUE;
	BOOL Comparison2 = TRUE;
	BOOL Comparison3 = TRUE;
	BOOL Comparison4 = TRUE;

	for (UINT64 i = 0; i < Amount; i++)
	{
		UINT_PTR* Src = (Source + i);
		UINT_PTR* Dst = (Dest + i);
		if (*Src != *Dst)
		{
			*(Val) = FALSE;
			break;
		}
		*(Val) = TRUE;
	}

	UINT32 Index = 0;
	for (Index = 0; Index < Amount; Index += 4)
	{
		Comparison1 &= (dstAsChar[Index + 0] == srcAsChar[Index + 0]);
		Comparison2 &= (dstAsChar[Index + 1] == srcAsChar[Index + 1]);
		Comparison3 &= (dstAsChar[Index + 2] == srcAsChar[Index + 2]);
		Comparison4 &= (dstAsChar[Index + 3] == srcAsChar[Index + 3]);
	}

	for (; Index < Amount; ++Index)
	{
		Comparison1 &= (dstAsChar[Index + 0] == srcAsChar[Index + 0]);
	}

	*(Val) = (Comparison1 && Comparison2 && Comparison3 && Comparison4);
	return STATUS_SUCCESS;
}

/* Same as above, but with bytes. */
FERALSTATUS FERALAPI KiSetMemoryBytes(
	INOUT VOID* Dest, IN UINT8 Val, IN UINT64 Amt)
{
	UINT8* RealDest = (UINT8*)Dest;
	/* TODO: Blocks of 8 to speed up performance again */
	for (UINT64 Index = 0; Index < Amt; ++Index)
	{
		RealDest[Index] = Val;
	}
	return STATUS_SUCCESS;
}