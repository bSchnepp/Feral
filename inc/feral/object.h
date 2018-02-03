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

#ifndef _FERAL_OB_H_
#define _FERAL_OB_H_

#include <feral/stdtypes.h>
#include <feral/string.h>

typedef enum FERAL_OBJECT_TYPE
{
	/* What kind of object are we looking at? */
	FERAL_FILE_OBJECT,
	FERAL_DRIVER_OBJECT,
	FERAL_TASK_OBJECT,	/* We use MACH terminology for what exactly is a program. */
	FERAL_PROCESS_OBJECT,
	FERAL_THREAD_OBJECT,
	FERAL_SEMAPHORE_OBJECT,
	FERAL_WAVEFRONT_OBJECT,	// for graphics stuff...
	FERAL_TIMER_OBJECT,
	FERAL_RENDER_BUFFER_OBJECT,	// Similar to Wayland buffers. (Internal drawing management does the actual GUI fun stuff... This is Qt or GTK+ or Pathfinder drawing the widgets.)
	FERAL_DESKTOP_OBJECT,
	FERAL_CLIPBOARD_OBJECT,
	FERAL_PROGRAM_OBJECT,
	FERAL_SERVICE_OBJECT,	/* A service is a userspace daemon which implements a specific set of functions requested by the kernel. (ie, a FUSE driver, or a printing service) */
	FERAL_PACKET_OBJECT,	/* A network packet (9P, IP, etc.) */
	FERAL_NETWORK_OBJECT,	/* Connection to some kind of network */
	
}FERAL_OBJECT_TYPE;

typedef struct FERAL_PERMISSION_DESCRIPTOR
{
	BOOLEAN Read;
	BOOLEAN Write;
	BOOLEAN Execute;
}FERAL_PERMISSION_DESCRIPTOR;

typedef enum FERAL_OBJECT_OPEN_REASON
{
	OBJECT_CREATE_HANDLE,	/* First time referencing this object... */

	OBJECT_OPEN_HANDLE,	/* Opening a reference to an already existing object */		
	/* Sidenode: if it's been updated on the system (ie, overwritten), the OS delivers the old version to running programs and new versions to subsequent programs, ala Linux. */

	OBJECT_CLONE_HANDLE,	/* Duplicated handle the program already has */

	OBJECT_INHERIT_HANDLE,	/* The parent task opened this, and allowed it's children to use it. */
}FERAL_OBJECT_OPEN_REASON;

typedef struct FERALOBJECT
{
	/* Number of references to this object. If KeFreeObject() is called and makes this 0, 
	   the kernel should deallocate this object and free everything inside of it.
	 */

	STRING ObjectName;
	FERAL_OBJECT_TYPE ObjectType;

	UINT64 NumReferences;
	UINT64 MaxReferences;
	UINTN  ReferenceTable;	// pointer
	
	UINT64 NumPermissionDescriptions;		/* For every object, we should create permission descriptors for everyone who should be able to get to them. */
	FERAL_PERMISSION_DESCRIPTOR* Permissions;	/* And then we have an array of them. (the structs are allocated on heap) */

	UINT64 MaxMemorySize;		/* Memory maximum */
	UINT64 DiskAllocMaximum;	/* Max number of blocks on the filesystem this object can utilize. */

	BOOL Pageable;			/* Can this object get thrown into swapfile if we need to? */
	
	UINT64 NumMethods;
	STRING* MethodNames;	/* Consider replacing with something else... */

	UINT64 RESERVED1;
	UINT64 RESERVED2;
	UINT64 RESERVED3;
	UINT64 RESERVED4;

	VOID* ObjectPointer;	/* Pointer to the object in question. */
}FERALOBJECT;


#endif
