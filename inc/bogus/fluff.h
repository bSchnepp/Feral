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

#ifndef _FERAL_BOGUS_FLUFF_H_
#define _FERAL_BOGUS_FLUFF_H_

/* This file just defines some fluff to make function headers look a little nicer and explain what exactly is going on. */

/* This is an input to a function. */
#define IN

/* This is an output of a function. */
#define OUT

/* This is expected as input, but will also be modified by the function. */
#define INOUT

/* This is optional input. Not required. */
#define INOPT


/* Yes, 'API' is not a typo, even though we're dealing with ABIs. This is because this is how the API would be called. Slightly confusing, yes, but I'm just following an established thing. */

#define FERALAPI __attribute__((cdecl))	//Team Red's ABI. (the one basically everyone else uses).

#define  MSAPI __attribute__(__declspec)
#define EFIAPI __attribute__(__declspec)

#endif
