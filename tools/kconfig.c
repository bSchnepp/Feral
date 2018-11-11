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
 
 
#include <stdlib.h>
#include <stdio.h>
#include <feral/stdtypes.h>

typedef struct Definition;

#define TRISTATE_NO	(-1)
#define TRISTATE_MOD	(0)
#define TRISTATE_YES	(1)

#define CONTENTS_STATUS_ARE_TRISTATE	(0)
#define CONTENTS_STATUS_ARE_STRING		(1)
#define CONTENTS_STATUS_ARE_INTEGER		(2)
#define CONTENTS_STATUS_ARE_OTHER		(3)

typedef struct Definition
{
	STRING Item;
	UINT32 contentsStatus;
	union
	{
		INT32 tristate;
		STRING string;
	}Contents;
	
	UINT64 numChildren;
	struct Definition* children;
}Definition;


Definition* ParseFile(STRING path)
{
	FILE* file = fopen(path, "r");
	if (!file)
	{
		return (Definition*)(0);
	}
	
	fclose(file);
}


void printUsage(void)
{
	fprintf(stdout, "Usage: kconfig.elf <FILE>\n");
}

int main(int argc, STRING *argv)
{
	if (argc < 2)
	{
		printUsage();
		return 0;
	}
	
	Definition* res = ParseFile(argv[1]);
	return 0;
}
