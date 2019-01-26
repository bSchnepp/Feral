/*
Copyright (c) 2018, Brian Schnepp

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

#ifndef _FERAL_KERN_VER_H_
#define _FERAL_KERN_VER_H_

#if defined(__cplusplus)
extern "C" {
#endif


// Increment whenever the kernel API changes signifigantly.
// For now, since we're unstable, we're *always* changing, so only actually bother once we get to version 1.0.0
// (we should increment to 0.0.2 when we have the _bare minimum_ features required to be "usable", ie, it can *a* user mode program.)


#define FERAL_VERSION_MAJOR (0000000)
#define FERAL_VERSION_MINOR (0000000)
#define FERAL_VERSION_PATCH (0000001)


#define FERAL_VERSION_STRING "FERAL Kernel 0.01 Release 'Alpha Aligator'"
#define FERAL_VERSION_SHORT "Alpha Aligator"

/* 
	Majority of *NIX out there is Linux, and the stuff that isn't Linux is mostly compatible with Linux 
	anyway. Exception are a few odd BSDs, but our toolchain is on Linux, and we need to make it think it's on Linux. 
*/
#define UNAME_NAME "Linux"
#define UNAME_VERSION "2038.01.19-44-feral" /* Something bogus for -r */



#if defined(__cplusplus)
}
#endif


#endif
