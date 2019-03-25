/*
Copyright (c) 2019, Brian Schnepp

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
 
#ifndef _FERAL_X86_64_PAGE_FLAGS_H_
#define _FERAL_X86_64_PAGE_FLAGS_H_

#define X86_PAGE_FLAG_PRESENT				(1 << 0)
#define X86_PAGE_FLAG_WRITABLE				(1 << 1)
#define X86_PAGE_FLAG_USER_READ				(1 << 2)
#define X86_PAGE_FLAG_WRITE_PAST_CACHE		(1 << 3)
#define X86_PAGE_FLAG_CACHE_DISABLED		(1 << 4)
#define X86_PAGE_ACCESSED_FLAG				(1 << 5)
#define X86_PAGE_DIRTY							(1 << 6)
#define X86_PAGE_HUGE							(1 << 7)
#define X86_PAGE_GLOBAL						(1 << 8)

#define X86_PAGE_NO_EXECUTE					(1 << 63)

#if defined(__i386__)
#define PAGE_ALIGN(x) (x & 0xFFFFF000)
#elif defined(__x86_64__)
#define PAGE_ALIGN(x) (x & 0xFFFFFFFFFFFFF000)
#endif

#endif
