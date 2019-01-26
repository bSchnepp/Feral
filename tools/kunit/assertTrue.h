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

#include <feral/stdtypes.h>
#include <feral/kern/frlos.h>

#ifndef _FERAL_K_UNIT_TEST_ASSERT_TRUE_
#define _FERAL_K_UNIT_TEST_ASSERT_TRUE_

/* 
	Personally I don't like finding out 4 months later there's a big bug in the kernel, so let's try to
	modularize ala NetBSD and cram kernel stuff into little user mode programs that simulate a specific
	subsystem of the Feral kernel (ie, useful for testing network stuff, ensuring this time I didn't screw up kprintf, etc. etc.) 
	
	Okay, maybe it's not quite like NetBSD, (we don't have a userspace yet in Feral, so we obviously can't jettison some kernel features to usermode and bring it back.)
	Feral has yet to run on actual hardware anyway, just qemu (kvm) and going to test with HVM on Xen soon.
*/

/* TODO: Clever way of tracking things? (open a  'debug context' or something???)*/
inline BOOL assertTrue(BOOL Condition)
{
	if (!Condition)
	{
		KiPrintWarnLine("Assertion was false");
	}
	return Condition;
}

inline BOOL assertFalse(BOOL Condition)
{
	if (Condition)
	{
		KiPrintWarnLine("Assertion was true");
	}
	return Condition;
}



#endif
