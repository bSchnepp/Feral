/*
Copyright (c) 2018, 2019, Brian Schnepp

Permission is hereby granted, free of charge, to any person or organization
obtaining  a copy of the software and accompanying documentation covered by
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

#ifndef _FERAL_FERAL_STDTYPES_H_
#define _FERAL_FERAL_STDTYPES_H_

#if defined(__cplusplus)
extern "C"
{
#endif

#include <stdint.h>

/*  Some function decorators.*/
/* This is an input to a function. */
#define IN

/* This is an output of a function. */
#define OUT

/* This is expected as input, but will also be modified by the function. */
#define INOUT

/* This is optional input. Not required. */
#define INOPT

/* Optional (other) */
#define OPT

/* May be null on return. */
#define OUTOPT

/* Optional, does something to this object. */
#define INOUTOPT

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/*
	Yes, 'API' is not a typo, even though we're dealing with ABIs.
	This is because this is how the API would be called. Slightly confusing,
	yes, but I'm just following an established thing.
*/

/* Team Red's ABI. (the one basically everyone else uses). */
#define FERALAPI __attribute__((sysv_abi))

#define MSAPI __attribute__((ms_abi))
#define WINAPI __attribute__((ms_abi))
#define EFIAPI __attribute__((ms_abi))

#define CONST const
#define INLINE inline
#define VOID void

/* Compilers in general are usually pretty terrible at 
 * inlining when you ask it to. Let's force it. */
#if (defined(__clang__) || defined(__GNUC__))
#define FORCE_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
/* Trying to compile Feral with MSVC will almost certainly fail. 
 * But you can try...
 */
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE inline
#endif

	typedef unsigned char BYTE;
	typedef BYTE BOOLEAN;
	typedef char CCHAR;
	typedef char CHAR;

#ifndef FERAL_NO_DEPRECATED
/*
	One should avoid DWORD as a word size may differ on various platforms
	(8-bit word, 32-bit words, etc.)
 */
#define PVOID void*
	typedef uint32_t DWORD;
	typedef uint64_t DWORDLONG;
#endif

	typedef uint32_t DWORD32;
	typedef uint64_t DWORD64;

	typedef float FLOAT;
	typedef double DOUBLE;

	typedef float FLOAT32;
	typedef double FLOAT64;

	typedef int32_t INT;
	typedef INT BOOL;
	typedef uint16_t WORD;

	typedef int8_t INT8;
	typedef int16_t INT16;
	typedef int32_t INT32;
	typedef int64_t INT64;

	typedef unsigned char UINT8;
	typedef uint16_t UINT16;
	typedef uint32_t UINT32;
	typedef uint64_t UINT64;

	typedef int32_t LONG;
	typedef int64_t LONGLONG;

/* We're LP64. */
#if defined(__x86_64__) || defined(__aarch64__)
	typedef int64_t INT_PTR;
	typedef uint64_t UINT_PTR;
#elif defined(__i386__)
	typedef int32_t INT_PTR;
	typedef uint32_t UINT_PTR;
#endif

	typedef INT_PTR LONG_PTR;

	typedef uint64_t QUAD;
	typedef QUAD QWORD;

#if defined(__x86_64__) || defined(__aarch64__)
	typedef uint64_t UINTN;
	typedef int64_t INTN;
#elif defined(__i386__)
typedef uint32_t UINTN;
typedef int32_t INTN;
#endif
	/*
	286 and earlier not supported.
	(16-bit x86 is *very* old and doesn't support paging).
 */


	/* Ensure we define wchar_t. This is important, as we *really* love
	 * Unicode. */
	typedef unsigned short wchar_t;
	typedef wchar_t* WSTRING;
	typedef wchar_t WCHAR;
	/* Also define WIDE CHAR and WIDE STRING. */

	typedef CHAR* STRING;

	typedef UINT32 COLORREF;
	typedef int16_t CSHORT;

	typedef UINT64 TIMEUNIT;

	typedef struct
	{
		UINT16 Year;
		UINT8 Month;
		UINT8 Day;
		UINT8 Hour;
		UINT8 Minute;
		UINT8 Second;
	} FERALTIME;

	typedef struct
	{
		UINT16 Year;
		UINT8 Month;
		UINT8 Day;
		UINT8 Hour;
		UINT8 Minute;
		UINT8 Second;
		UINT8 Millisecond;
		UINT8 Microsecond;
		UINT8 Nanosecond;
	} FERALTIMEHARD;

	typedef struct GUID
	{
		UINT32 Data1;
		UINT16 Data2;
		UINT16 Data3;
		UINT8 Data4[8];
	} GUID;

/* Please avoid using this. (some archs don't like packed structs) */
#define PACKED __attribute__((packed))


#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef NULLPTR
#define NULLPTR NULL
#endif

typedef void* POINTER;

#if defined(__cplusplus)
}
#endif

#include <feral/feraluser.h>


/* TEMPORARY FIXME */
UINT_PTR __stack_chk_guard;
#endif
