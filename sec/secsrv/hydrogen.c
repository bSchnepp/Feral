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
 
#include <feral/stdtypes.h>
#include <sec/secsrv/hydrogen.h>

BOOL CheckSecurityClearance(SecurityClearanceLevel userHas, SecurityClearanceLevel wanted)
{
	/* Use a bitwise AND to check the difference. If user has lower, 
	   then some bits are not set in lower region.
	   If a user has higher permissions, it's ignored, and allowed anyway.
	*/
	UINT64 res = (UINT8)(userHas & wanted);
	UINT64 wantedAsBits = (UINT8)(wanted);
	/* 
		Take advantage that BOOL is just an integer, and we can do this 
		to avoid a branch. This keeps us safer from side-channel 
		timing-based attacks.
		
		If they're not the same, subtract gives something not zero.
		If they are, then it becomes zero. Then, we invert the
		result to give back what we actually want (it to be true iff 
		zero) 
	*/
	return !(wantedAsBits - res);
}
