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

// Reimplementation of the Vulkan header, so we can guarantee at least a baseline version.
// We're supporting at the very least version 1.0.68. Hardware incapable of Vulkan support should not run Feral Waypoint.

#ifndef _VULKAN_H_
#define _VULKAN_H_

#define FRL_EXTENSIONS
#define VK_VERSION_1_0

// We utilize the default way of doing this. (Nothing special needed).
#define VKAPI_ATTR
#define VKAPI_CALL
#define VKAPI_PTR

#include <feral/stdtypes.h>
#include <stddef.h>
#include <stdint.h>

// For the relevant info...
#ifdef VK_USE_PLATFORM_WAYPOINT_FRL
#include <waypoint.h>
#endif



#ifdef __cplusplus
extern "C" {
#endif

#define VK_MAKE_VERSION(maj, min, pch) (( (maj) << 22) | ( (min) << 12) | (pch))

#define VK_API_VERSION_1_0 VK_MAKE_VERSION(1, 0, 0)	// For the base version

//TODO

#ifdef __cplusplus
}
#endif

