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

#ifndef _FERAL_IPC_PORT_H_
#define _FERAL_IPC_PORT_H_

#include <feral/stdtypes.h>
#include <feral/feraluser.h>

typedef enum
{
	PORT_TYPE_RECIEVE,
	PORT_TYPE_SEND,
	PORT_TYPE_SEND_ONCE,
	PORT_TYPE_BIDIRECTIONAL,
}PORT_PURPOSE;

typedef enum
{
	PORT_RIGHT_CREATE,
	PORT_RIGHT_DELETE.
	PORT_RIGHT_LINK,
	PORT_RIGHT_CREATE_LINK,
	PORT_RIGHT_DELETE_LINK,
	PORT_RIGHT_ALL,
}PORT_CREATION_RIGHT;

typedef struct FERALPORT 
{
	UINT64 NumUsers;
	FERALUSER* Users;

	FERALUSER Origin;
	VOID* Data;
}FERALPORT, *PFERALPORT;




#endif
