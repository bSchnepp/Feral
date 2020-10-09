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

#include <stdio.h>

#include <feral/stdtypes.h>
#include <feral/feralstatus.h>

#ifndef _TEST_DEFINES_H_
#define _TEST_DEFINES_H_

typedef int TEST_STATUS;

UINT64 TEST_INTERNAL_LengthOne = 0;
UINT64 TEST_INTERNAL_LengthTwo = 0;
UINT64 TEST_INTERNAL_Lesser = 0;

#define TEST_STATUS_OK ((TEST_STATUS)(0))
#define TEST_STATUS_ERR ((TEST_STATUS)(-1))

#define ASSERT_EQ_LLD(x, y, TestName, TestCategory) \
	if (x != y) \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;31mFAILED TEST: %lld not equal to " \
			"%lld\033[0m\n", \
			x, y); \
		return TEST_STATUS_ERR; \
	} \
	else \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;32mPASSED TEST: %lld is equal to " \
			"%lld\033[0m\n", \
			x, y); \
	}

#define ASSERT_NEQ_LLD(x, y, TestName, TestCategory) \
	if (x == y) \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;31mFAILED TEST: %lld not equal to " \
			"%lld\033[0m\n", \
			x, y); \
		return TEST_STATUS_ERR; \
	} \
	else \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;32mPASSED TEST: %lld not equal to " \
			"%lld\033[0m\n", \
			x, y); \
	}

#define ASSERT_EQ_STR(x, y, TestName, TestCategory) \
	TEST_INTERNAL_LengthOne = strlen(x); \
	TEST_INTERNAL_LengthTwo = strlen(y); \
	TEST_INTERNAL_Lesser \
		= (TEST_INTERNAL_LengthOne < TEST_INTERNAL_LengthTwo) \
			  ? TEST_INTERNAL_LengthOne \
			  : TEST_INTERNAL_LengthTwo; \
	if ((strncmp(x, y, TEST_INTERNAL_Lesser) != 0) \
		|| (TEST_INTERNAL_LengthOne != TEST_INTERNAL_LengthTwo)) \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;31mFAILED TEST: %s not equal to %s\033[0m\n", \
			x, y); \
		return TEST_STATUS_ERR; \
	} \
	else \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;32mPASSED TEST: %s is equal to %s\033[0m\n", \
			x, y); \
	}

#define ASSERT_NEQ_STR(x, y, TestName, TestCategory) \
	UINT64 TEST_INTERNAL_LengthOne = strlen(x); \
	UINT64 TEST_INTERNAL_LengthTwo = strlen(y); \
	TEST_INTERNAL_Lesser \
		= (TEST_INTERNAL_LengthOne < TEST_INTERNAL_LengthTwo) \
			  ? TEST_INTERNAL_LengthOne \
			  : TEST_INTERNAL_LengthTwo; \
	if ((strncmp(x, y, TEST_INTERNAL_Lesser) == 0) \
		&& (TEST_INTERNAL_LengthOne == TEST_INTERNAL_LengthTwo)) \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;31mFAILED TEST: %s is equal to %s\033[0m\n", \
			x, y); \
		return TEST_STATUS_ERR; \
	} \
	else \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;32mPASSED TEST: %s not equal to %s\033[0m\n", \
			x, y); \
	}
	
#define ASSERT_EQ_MEM(x, y, length, TestName, TestCategory) \
	TEST_INTERNAL_Lesser = (length); \
	if ((memcmp(x, y, TEST_INTERNAL_Lesser) != 0) \
		|| (TEST_INTERNAL_LengthOne != TEST_INTERNAL_LengthTwo)) \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;31mFAILED TEST: %p not equal to %p\033[0m\n", \
			x, y); \
		return TEST_STATUS_ERR; \
	} \
	else \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;32mPASSED TEST: %p is equal to %p\033[0m\n", \
			x, y); \
	}

#define ASSERT_NEQ_MEM(x, y, length, TestName, TestCategory) \
	TEST_INTERNAL_Lesser = (length); \
	if ((memcmp((char*)x, (char*)y, TEST_INTERNAL_Lesser) == 0) \
		&& (TEST_INTERNAL_LengthOne == TEST_INTERNAL_LengthTwo)) \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;31mFAILED TEST: %p is equal to %p\033[0m\n", \
			x, y); \
		return TEST_STATUS_ERR; \
	} \
	else \
	{ \
		fprintf(stderr, "In test %s of category %s: \n", TestName, \
			TestCategory); \
		fprintf(stderr, \
			"\033[1;32mPASSED TEST: %p not equal to %p\033[0m\n", \
			x, y); \
	}


#define ASSERT_TRUE(x, TestName, TestCategory) \
	ASSERT_EQ_LLD((long long)x, TRUE, TestName, TestCategory);
#define ASSERT_FALSE(x, TestName, TestCategory) \
	ASSERT_EQ_LLD((long long)x, FALSE, TestName, TestCategory);


#endif
