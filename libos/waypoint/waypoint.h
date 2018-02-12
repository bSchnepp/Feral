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

#ifndef _WAYPOINT_H_
#define _WAYPOINT_H_

#include <feral/stdtypes.h>
#include <bogus/fluff.h>

#define FERAL_MAKE_VERSION(a, b) ((a << 16) | (b))	//TODO, consider changing this to (<< 22), ( << 12), | patch, like VK_MAKE_VERSION

typedef enum
{
	// 'generic' is there just to describe the program running.
	FERAL_GENERIC_TERMINAL_APPLICATION,
	FERAL_GENERIC_GUI_APPLICATION,
	FERAL_GENERIC_GAME_APPLICATION,
	FERAL_GENERIC_MEDIA_APPLICATION,
	FERAL_GENERIC_WEB_BROWSER_APPLICATION,
	FERAL_GENERIC_APPLICATION,

	// These have special meanings.
	FERAL_EMULATED_GAME_APPLICATION,	// This game is running on a libos emulated processor.
	FERAL_NONCRITICAL_GAME_APPLICATION,	// This game is not heavy on resources, and thus is OK to de-prioritize.
}ApplicationType;

// TODO: do this better (this should be part of libc / c runtime)
// (in order: argc, argv, Parent task, owning user, and current directory)
UINT32 WayMain(IN UINT32 ArgumentCount, IN WSTRING* Arguments, IN HTASK Parent, IN HUSER User, IN HANDLE Directory);

typedef UINT64 WPSTATUS;

typedef struct SystemProperties
{
	// TODO...
}SystemProperties;

typedef struct ApplicationProperties
{
	SystemProperties SystemProps;
	ApplicationType AppPurpose;
	UINT32 AppVersion;
	UINT64 RESERVED;
}ApplicationProperties;

BOOL CreateApplicationFrame(WSTRING Title, HUSER User);








#endif
