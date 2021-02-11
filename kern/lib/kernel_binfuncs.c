/*
Copyright (c) 2020, 2021, Brian Schnepp

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

#include <feral/kern/krnlbinfuncs.h>

/**
 * @brief Takes the logarithm of the absolute value of a number by the base 2.
 * That is, 2 can be multiplied by itself by the number returned in Result,
 * and the resulting number from that will be the nearest power of two, rounding
 * downwards from the original number. As such, KiLog2Int(-4096) will have the
 * same result as KiLog2Int(8000)
 *
 * @author Brian Schnepp
 * @param Input The input number, as an signed 64-bit integer
 * @param Result A pointer to the result of this operation
 *
 */
FERALSTATUS FERALAPI KiLog2Int(IN INT64 Input, OUT INT64 *Result)
{
	INT64 ResultOne = 0;
	INT64 ResultTwo = 0;

	/* Attempt both, then return the correct one. */
	KiLog2UInt((UINT64)(Input), &ResultOne);
	KiLog2UInt((UINT64)(-Input), &ResultOne);

	/* And use branchless programming to pick between them. */
	(*Result) = ((Input <= 0) * (ResultTwo)) + ((Input > 0) * (ResultOne));
	return STATUS_SUCCESS;
}

/**
 * @brief Takes the logarithm of a given number by the base 2.
 * That is, 2 can be multiplied by itself by the number returned in Result,
 * and the resulting number from that will be the nearest power of two, rounding
 * downwards from the original number. As such, KiLog2UInt(4096) will have the
 * same result as KiLog2UInt(8000)
 *
 * @author Brian Schnepp
 * @param Input The input number, as an unsigned 64-bit integer
 * @param Result A pointer to the result of this operation
 *
 */
FERALSTATUS FERALAPI KiLog2UInt(IN UINT64 Input, OUT INT64 *Result)
{
	INT64 Res = 0;

	/* Terribly slow. */
	UINT64 Copy = Input;
	while (Copy >= 2)
	{
		Res++;
		Copy /= 2;
	}

	(*Result) = Res;
	return STATUS_SUCCESS;
}

/* Internal function: suppress warning (for now) */
VOID internalItoa(UINT64 val, CHAR *buf);
VOID internalSignedItoa(INT64 val, CHAR *buf);
VOID internalItoaBaseChange(UINT64 val, CHAR *buf, UINT8 radix);


VOID internalItoaBaseChange(UINT64 val, CHAR *buf, UINT8 radix)
{
	UINT64 len = 0;
	/* No point in continuing if it's zero. Just give '0' back. */
	if (val == 0)
	{
		*buf = '0';
		*(buf + 1) = '\0';
		return;
	}
	/* Weird behavior if you exceed 10 + 26 + 26 as radix. */
	for (UINT64 valCopy = val; valCopy != 0; valCopy /= radix)
	{
		/* We need the remainder to see how to encode. */
		CHAR rem = valCopy % radix;
		if (rem <= 9 && rem >= 0)
		{
			/* It's already a number. Just encode in ASCII. */
			buf[len++] = rem + '0';
		}
		else if (rem < 35)
		{
			/* Encode the number as a lowercase letter. */
			buf[len++] = (rem - 10) + 'a';
		}
		else
		{
			/* Encode as uppercase. */
			buf[len++] = (rem - 36) + 'A';
		}
	}

	/* It's written BACKWARDS. So flip the order of the string. */
	for (UINT64 i = 0; i < len / 2; ++i)
	{
		CHAR tmp = buf[i];
		buf[i] = buf[len - i - 1];
		buf[len - i - 1] = tmp;
	}

	/* Terminate the string. */
	buf[len] = '\0';
}

/*
	We don't have a kmalloc() yet, so we can't just free and alloc whenever
   we want. Thus, our internal itoa _must_ be in-place.

	buf must be at least size 2, and is at most going to use 21 characters.
*/
VOID internalItoa(UINT64 val, CHAR *buf)
{
	internalItoaBaseChange(val, buf, 10);
}



/*
	We don't have a kmalloc() yet, so we can't just free and alloc whenever
   we want. Thus, our internal itoa _must_ be in-place.

	buf must be at least size 2, and is at most going to use 22 characters.
*/
VOID internalSignedItoa(INT64 val, CHAR *buf)
{
	UINT64 len = 0;
	UINT64 base = 10;

	if (val >= 0)
	{
		return internalItoa(val, buf);
	}

	/* We can assume we're negative: now we can avoid doing / or %
	 * on a negative value.
	 */
	for (INT64 overzero = 0 - val; overzero != 0; overzero /= base)
	{
		CHAR rem = overzero % base;
		buf[len++] = rem + '0';
	}

	/* Append the -. Put it at the END since it'll be flipped soon. */
	buf[len++] = '-';

	/* Flip the string around. */
	for (UINT64 i = 0; i < len / 2; ++i)
	{
		CHAR tmp = buf[i];
		buf[i] = buf[len - i - 1];
		buf[len - i - 1] = tmp;
	}

	/* Terminate the string. */
	buf[len] = '\0';
}

FERALSTATUS FERALAPI KiItoa(IN INT64 Input, OUT CHAR *Result)
{
	internalItoa(Input, Result);
	return STATUS_SUCCESS;
}

FERALSTATUS FERALAPI KiItoaSigned(IN INT64 Input, OUT CHAR *Result)
{
	internalSignedItoa(Input, Result);
	return STATUS_SUCCESS;
}

FERALSTATUS FERALAPI KiItoaBase(IN UINT64 Input, UINT8 Base, OUT CHAR *Result)
{
	internalItoaBaseChange(Input, Result, Base);
	return STATUS_SUCCESS;
}