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

#include <libreefi/efi.h>
#include <feral/stdtypes.h>

static EFI_HANDLE ImageHandle;
static EFI_SYSTEM_TABLE* SystemTable;


static EFI_GUID GuidEfiLoadedImageProtocol = EFI_LOADED_IMAGE_PROTOCOL_GUID;
static EFI_GUID GuidEfiSimpleFSProtocol = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
static EFI_GUID GuidEfiFileInfoGuid = EFI_FILE_INFO_ID;

/* TODO: Refactor into something nice. */

EFI_STATUS EFIAPI uefi_main(EFI_HANDLE mImageHandle, EFI_SYSTEM_TABLE* mSystemTable)
{
	UINTN MapSize = 0;
	UINTN MapKey = 0;
	UINTN DescriptorSize = 0;
	UINT32 DescriptorVersion = 0;
	EFI_STATUS Result = EFI_SUCCESS;
	EFI_MEMORY_DESCRIPTOR *MemoryMap = NULLPTR;

	EFI_FILE_PROTOCOL *Root = NULLPTR;
	EFI_FILE_PROTOCOL *KernelImage = NULLPTR;
	EFI_FILE_PROTOCOL *ESPRoot = NULLPTR;
	
	EFI_LOADED_IMAGE_PROTOCOL *LoadedImageProtocol = NULLPTR;
	EFI_SIMPLE_FILESYSTEM_PROTOCOL *FileSysProtocol = NULLPTR;
	EFI_HANDLE_PROTOCOL HandleProtocol 
		= SystemTable->BootServices->HandleProtocol;

	ImageHandle = mImageHandle;
	SystemTable = mSystemTable;
	
	SystemTable->ConOut->OutputString(SystemTable->ConOut, 
		L"Starting Feralboot...\r\n");

	/* Get the UEFI memory stuff. */
	Result = SystemTable->BootServices->GetMemoryMap(&MapSize, MemoryMap,
		NULL, &DescriptorSize, NULL); 
		
	if (Result == EFI_BUFFER_TOO_SMALL)
	{
		/* TODO... */
	}
	
	/* Add some area to actually hold the info. */
	MapSize += (DescriptorSize << 1);
	Result = SystemTable->BootServices->AllocatePool(EfiLoaderData, MapSize,
		(void**)(&MemoryMap)
	);
	if (Result != EFI_SUCCESS)
	{
		/* TODO... */
	}
	
	Result = SystemTable->BootServices->GetMemoryMap(&MapSize, MemoryMap, 
		&MapKey, &DescriptorSize, &DescriptorVersion
	);
	
	if (Result != EFI_SUCCESS)
	{
		/* TODO... */
	}
	
	/* Check for protocols necessary to mess with filesystem. */
	Result = HandleProtocol(ImageHandle, &GuidEfiSimpleFSProtocol,
		(void**)(&LoadedImageProtocol));
	if (Result != EFI_SUCCESS)
	{
		/* TODO... */
	}
	
	Result = HandleProtocol(ImageHandle, &GuidEfiLoadedImageProtocol,
		(void**)(&FileSysProtocol));
	if (Result != EFI_SUCCESS)
	{
		/* TODO... */
	}

		
	/* Load /EFI/Feral/FERALKER.NEL to do things! */
	
	Result = FileSysProtocol->OpenVolume(FileSysProtocol, &ESPRoot);
	
	if (Result != EFI_SUCCESS)
	{
		SystemTable->ConOut->OutputString(SystemTable->ConOut, 
			L"ESP could not be initialized...\r\n");
		return Result;
	}


	
	Result = ESPRoot->Open(ESPRoot, &KernelImage, 
		L"/EFI/Feral/FERALKER.NEL", EFI_FILE_MODE_READ, 
		EFI_FILE_READ_ONLY
	);
	
	if (Result != EFI_SUCCESS)
	{
		SystemTable->ConOut->OutputString(SystemTable->ConOut, 
			L"Kernel could not be initialized...\r\n");
		return Result;
	}
		
	
	/* Terminate boot services (about to execute kernel) */
	SystemTable->ConOut->OutputString(SystemTable->ConOut, 
		L"Terminating firmware services...\r\n");

	
	/* Start loading of the kernel. (setup and call KiSystemStartup) */


	SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
	
	return EFI_SUCCESS;
}
