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

#include <feral/feralstatus.h>
#include <feral/stdtypes.h>
#include <feral/object.h>

#include <ddk/frlddk.h>
#include <ddk/frldev.h>
#include <bogus/fluff.h>

FERALSTATUS KeCreateDriver(INOUT FERAL_DRIVER* Target, 
	FERALSTATUS (*DriverInit)(IN DriverObject* Object, IN WSTRING RmsPath),
	FERALSTATUS (*DriverExit)(VOID),
	FERALSTATUS (*DriverDispatch)(UINT64 NumArgs, VOID** Stuff))
{
	if (DriverInit == ((void*)(0)) || DriverDispatch = ((void*)(0)))
	{
		return STATUS_ERROR;
	}
	
	Target->DriverInit = DriverInit;
	Target->DriverExit = DriverExit;
	Target->DriverDispatch = DriverDispatch;
	return STATUS_SUCCESS;
}

FERALSTATUS KeModifyDriverPriority(INOUT FERAL_DRIVER* Target, UINT8 NewPriority)
{
	
}

#if 0
typedef struct DriverObject
{
	HANDLE Object;
	STRING DeviceName;
}DriverObject;

typedef struct FERAL_DRIVER
{
	// Please use 'Boost', 'GPLv2', 'GPLv3', 'MPLv2', 'MIT', 'BSD 4-clause', 'BSD 3-clause', 
	// "Proprietary", etc., and use 'DUAL:[<LICENSE1>,<LICENSE2>]' for dual-licenses.
	// We don't actually _do anything_ with that, it's just so that when loaded, you can see what is non-free
	// and whatever if you care about that sort of stuff.
	STRING LicenceName;

	// Reference to the object we're controlling in question.
	FERALOBJECT Object;


	/* These functions _must_ be present in all drivers. They can never be null (or we unload/ignore the driver) */

	// The main function as the kernel calls.
	// The kernel passes a DriverObject to it, sees if the driver likes it, along with the associated configuration
	// in the record mangement system if applicable.
	FERALSTATUS (*DriverInit)(IN DriverObject* Object, IN WSTRING RmsPath);

	// Destructor for the driver.
	// This is called whenever the kernel intends to either restart or outright shut down the driver.
	// (For example, a driver fault, and the kernel tries to save the system by shutting down the driver.)
	// Keep in mind this is in kernel-space, and you should free whatever you allocated.
	// You should also check for failure and all, ensure you do _not_ corrupt memory.
	// Your driver may also be unloaded if a higher priority driver is detected later on during boot time.
	// Ensure your priority is correct.
	FERALSTATUS (*DriverExit)(VOID);

	// Arbitrary input (can be anything!!!)
	FERALSTATUS (*DriverDispatch)(UINT64 NumArgs, VOID** Stuff);

	
	UINT64 DriverFlags;	// TODO: Define.

	UINT8 DriverPriority;	// Value between 0 and 255 in which the device gets priority.
				// (This is so we can have a VGA driver as '1' to support all graphics devices, 
				// a VESA driver as '2', and a specific driver for a specific GPU of a specific make as '255'.	
}FERAL_DRIVER;

FERALSTATUS KeCreateDriver(INOUT FERAL_DRIVER* Target, 
	FERALSTATUS (*DriverInit)(IN DriverObject* Object, IN WSTRING RmsPath),
	FERALSTATUS (*DriverExit)(VOID),
	FERALSTATUS (*DriverDispatch)(UINT64 NumArgs, VOID** Stuff));

FERALSTATUS KeModifyDriverPriority(INOUT FERAL_DRIVER* Target, UINT8 NewPriority);
#endif

