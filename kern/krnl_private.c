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


// This header handles all the kernel private stuff (aka very volatile things)
// Essentially, you shouldn't mess with private kernel stuff other than through functions which do it for you.
// (don't ever include anything in inc/krnl.h, because your driver may break binary compatibility in the future.)
// (even then, I might end up changing the ABI anyway for some reason... if this happens, it'd probably end up with a major version change.)

#include "inc/krnl.h"

#define TEMPORARY_SERIAL_DRIVER_STUB

#include <feral/feralstatus.h>
#include <feral/stdtypes.h>
#include <feral/kern/frlos.h>
#include <feral/kern/ki.h>


/* TODO: Implement randomization for this. */
#if defined(__i386__) || defined(__arm__)
#else
	#include <arch/x86_64/vga/vga.h>
	#define STACK_CHK_GUARD 0x23C72A7D59AA6F2D
#endif

UINT_PTR __stack_chk_guard = STACK_CHK_GUARD;


FERALSTATUS KiLoadAllDrivers(VOID)
{
	// bogus stub for now (we don't have .SO loading ready yet to actually be of real use.)
#ifdef TEMPORARY_SERIAL_DRIVER_STUB
	// Hold on, we have a serial driver directly injected into the kernel rather than as a shared object.
	// This is for debugging purposes for now (we just throw output to COM1 for now.)
	
#endif

	return STATUS_SUCCESS;
}




#if defined(__x86_64__)
#include <arch/x86_64/cpufuncs.h>
VOID KiRestoreInterrupts(BOOLEAN value)
{
	if (value)
	{
		__asm__ __volatile__ ("sti");
	}
	else
	{
		__asm__ __volatile__ ("cli");
	}
}

#endif
