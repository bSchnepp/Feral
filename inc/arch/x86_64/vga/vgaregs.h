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

#ifndef _FERAL_KERNEL_VGA_REGISTERS_H_
#define _FERAL_KERNEL_VGA_REGISTERS_H_

#if defined(__cplusplus)
extern "C" {
#endif

#define VGA_MISC_OUTPUT_REG 0x03C2

#define VGA_FB_COMMAND_PORT 0x03D4
#define VGA_FB_DATA_PORT    0x03D5

#define VGA_HIGH_BYTE_COMMAND 0x0E
#define VGA_LOW_BYTE_COMMAND  0x0F

/* VGA video resolutions... */
#define VGA_VIDEO_MODE_80_25	0x0F00

/* I'm missing something here...  */
#define VGA_VIDEO_MODE_80_43	0x0F02
#define VGA_VIDEO_MODE_80_23	0x0F03

#if defined(__cplusplus)
}
#endif


#endif
