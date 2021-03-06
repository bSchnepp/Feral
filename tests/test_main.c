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

#include "defines.h"

#include "x86-stub.h"

#include <feral/stdtypes.h>
#include <feral/feralstatus.h>
#include <feral/kern/krnlmemfuncs.h>
#include <feral/kern/krnlstringfuncs.h>

#include <stdlib.h>
#include <string.h>

/* stubs for some assembly stuff the kernel expects. Maybe fixme?
 */
void *kern_start;
void *kern_end;

typedef TEST_STATUS (*TestCase)(VOID);

struct SomeStruct
{
	int Value;
	char SomeOther[4];
	long long int Another[11];
};

TEST_STATUS KernelMemCpy()
{
	struct SomeStruct k1 = {0};
	struct SomeStruct k2 = {4, {'a', 'b', 'c', 'd'}, {11}};
	KiCopyMemory(&k2, &k1, sizeof(struct SomeStruct));
	ASSERT_EQ_MEM(&k1, &k2, sizeof(struct SomeStruct), "KernelLib",
		"KiCopyMemory");
	return TEST_STATUS_OK;
}

TEST_STATUS KernelMemSet()
{
	struct SomeStruct k1 = {0};
	struct SomeStruct k2 = {4, {'a', 'b', 'c', 'd'}, {11}};
	KiSetMemoryBytes(&k2, 0, sizeof(struct SomeStruct));
	ASSERT_EQ_MEM(&k1, &k2, sizeof(struct SomeStruct), "KernelLib",
		"KiSetMemoryBytes");
	return TEST_STATUS_OK;
}

TEST_STATUS KernelMemSetNonZero()
{
	char Vals[4] = {79, 79, 79, 79};
	char Vals2[4];
	KiSetMemoryBytes(&Vals2, 79, 4);
	ASSERT_EQ_MEM(&Vals, &Vals2, 4, "KernelLib", "KiSetMemoryBytesNonZero");
	return TEST_STATUS_OK;
}

TEST_STATUS KernelCStringLength()
{
	STRING Val1 = "abcdef";
	STRING Val2 = "fedcba";

	UINT64 Len1 = 0;
	UINT64 Len2 = 0;

	KiGetStringLength(Val1, &Len1);
	KiGetStringLength(Val2, &Len2);

	ASSERT_EQ_LLD(Len1, Len2, "KernelLib", "KiGetStringLength");
	return TEST_STATUS_OK;
}

TEST_STATUS KernelCStringLengthDiff()
{
	STRING Val1 = "abcdef";
	STRING Val2 = "fedcba0";

	UINT64 Len1 = 0;
	UINT64 Len2 = 0;

	KiGetStringLength(Val1, &Len1);
	KiGetStringLength(Val2, &Len2);

	ASSERT_NEQ_LLD(Len1, Len2, "KernelLib", "KiGetStringLengthDiff");
	return TEST_STATUS_OK;
}

TEST_STATUS KernelCompareMemory()
{
	struct SomeStruct k1 = {0};
	struct SomeStruct k2 = {4, {'a', 'b', 'c', 'd'}, {11}};

	BOOL Value = 0;
	KiCopyMemory(&k2, &k1, sizeof(struct SomeStruct));
	KiCompareMemory(&k2, &k1, sizeof(struct SomeStruct), &Value);
	ASSERT_TRUE(Value, "KernelLib", "KiCompareMemory");
	return TEST_STATUS_OK;
}

TEST_STATUS KernelCompareMemoryNeg()
{
	struct SomeStruct k1 = {0};
	struct SomeStruct k2 = {4, {'a', 'b', 'c', 'd'}, {11}};

	BOOL Value = 0;
	KiCompareMemory(&k2, &k1, sizeof(struct SomeStruct), &Value);

	ASSERT_FALSE(Value, "KernelLib", "KiCompareMemory");
	return TEST_STATUS_OK;
}

TestCase Cases[] = {
	KernelCStringLength,
	KernelCStringLengthDiff,
	KernelMemCpy,
	KernelMemSet,
	KernelCompareMemory,
	KernelCompareMemoryNeg,
	KernelMemSetNonZero,
};

/* function pointer array is uniform size, unlike something like utf-8,
 * so every entry is the same size. Go ahead and use that to get sizeof array.
 */
#define UNIFORM_SIZE(x) (sizeof(x) / sizeof(x[0]))

int main(int argc, char **argv)
{
	for (UINT64 Index = 0; Index < UNIFORM_SIZE(Cases); ++Index)
	{
		TestCase Func = Cases[Index];
		TEST_STATUS Status = Func();
	}
	return 0;
}
