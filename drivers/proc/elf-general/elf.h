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

#ifndef _FERAL_PROC_ELF_H_
#define _FERAL_PROC_ELF_H_

#include <feral/stdtypes.h>

#pragma pack(1)

typedef enum
{
	ELF_RELOCATABLE = 0,
	ELF_EXECUTABLE = 1,
	ELF_SHARED = 2,
	ELF_CORE = 3
}ElfExecTypes;

typedef struct _ElfHeader
{
	UINT32 magic;	//ELF magic number...
	UINT8  cpu_bitsz;
	UINT8  cpu_endian;
	UINT8  e_version;
	UINT8  e_identifier;
	UINT64 e_padding;
	UINT16 e_type;
	UINT16 e_machine;
	UINT32 e_version;
	UINTN  e_entry;
	UINTN  e_phoff;
	UINTN  e_shoff;
	UINT32 e_flags;
	UINT16 e_ehsize;
	UINT16 e_phentsize;
	UINT16 e_phnum;
	UINT16 e_shentsize;
	
}ElfHeader;
#pragma pack(0)

#endif
