/* A teenty tiny bit of dirty #includes could be done to build a unit testing
 * framework.
 *
 * Maybe #include both the .c and the .h file, so that it's isolated within
 * one file, compile it in Linux userland, and then run them before the CI
 * script releases a nightly build?
 */

#define ASSERT_IS_EQUAL(x, y, Val) (if ((x) != (y)) { Val = 0 } else {Val = 1})

#include <feral/stdtypes.h>

bool Test_PageLevelsWhenEmpty()
{
	UINT64 Addr = 0;
	BOOL Okay = 0;
	ASSERT_IS_EQUAL(Addr, 0, Okay);
	return Okay;
}


int main()
{
	if (!Test_PageLevelsWhenEmpty())
	{
		return -1;
	}
	return 0;
}
