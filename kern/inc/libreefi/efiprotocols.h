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
 
#include <libreefi/efistatus.h>
#include <libreefi/eficommon.h> 

#ifndef _FERAL_LIBRE_EFI_PROTOCOLS_H_
#define _FERAL_LIBRE_EFI_PROTOCOLS_H_

#define EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL 0x00000001
#define EFI_OPEN_PROTOCOL_GET_PROTOCOL 0x00000002
#define EFI_OPEN_PROTOCOL_TEST_PROTOCOL 0x00000004
#define EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER 0x00000008
#define EFI_OPEN_PROTOCOL_BY_DRIVER 0x00000010
#define EFI_OPEN_PROTOCOL_EXCLUSIVE 0x00000020
#define EFI_LOADED_IMAGE_PROTOCOL_REVISION 0x0001000

#define EFI_FILE_PROTOCOL_REVISION 0x00010000
#define EFI_FILE_PROTOCOL_REVISION2 0x00020000

#define EFI_FILE_PROTOCOL_LATEST_REVISION EFI_FILE_PROTOCOL_REVISION2

#define EFI_DISK_IO_PROTOCOL_REVISION 0x00010000

#define EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID 		\
{							\
	0x0964E5B22,					\
	0x6459,					\
	0x11D2,					\
	0x8E,						\
	0x39,						\
	0x00,						\
	0xA0,						\
	0xC9,						\
	0x69,						\
	0x72,						\
	0x3B,						\
}

#define EFI_LOADED_IMAGE_PROTOCOL_GUID 		\
{							\
	0x5B1B31A1,					\
	0x9562,					\
	0x11D2,					\
	0x8E,						\
	0x3F,						\
	0x00,						\
	0xA0,						\
	0xC9,						\
	0x69,						\
	0x72,						\
	0x3B,						\
}

#define EFI_LOADED_IMAGE_DEVICE_PATH_PROTOCOL_GUID 	\
{							\
	0xBC62157E,					\
	0x3E33,					\
	0x4FEC,					\
	0x99,						\
	0x20,						\
	0x2D,						\
	0x3B,						\
	0x36,						\
	0xD7,						\
	0x50,						\
	0xDF,						\
}

#define EFI_FILE_INFO_ID				\
{							\
	0x09576E92,					\
	0x6D3F,					\
	0x11D2,					\
	0x39,						\
	0x8E,						\
	0x00,						\
	0xA0,						\
	0xC9,						\
	0x69,						\
	0x72,						\
	0x3B,						\
}

#define EFI_FILE_SYSTEM_INFO_ID 			\
{							\
	0x09576E93,					\
	0x6D3F,					\
	0x11D2,					\
	0x39,						\
	0x8E,						\
	0x00,						\
	0xA0,						\
	0xC9,						\
	0x69,						\
	0x72,						\
	0x3B,						\
}

#define EFI_FILE_SYSTEM_VOLUME_LABEL_ID 		\
{							\
	0xDB47D7D3,					\
	0xFE81,					\
	0x11D3,					\
	0x35,						\
	0x9A,						\
	0x00,						\
	0x90,						\
	0x27,						\
	0x3F,						\
	0xC1,						\
	0x4D,						\
}

#define EFI_DISK_IO_PROTOCOL_GUID 			\
{							\
	0xCE345171,					\
	0xBA0B,					\
	0x11D2,					\
	0x8E,						\
	0x4F,						\
	0x00,						\
	0xA0,						\
	0xC9,						\
	0x69,						\
	0x72,						\
	0x3B,						\
}

typedef struct _EFI_LOADED_IMAGE_PROTOCOL
{
	UINT32 Revision;
	EFI_HANDLE ParentHandle;
	EFI_SYSTEM_TABLE *SystemTable;
	EFI_HANDLE DeviceHandle;
	EFI_DEVICE_PATH_PROTOCOL *FilePath;
	VOID *RESERVED1;
	UINT32 LoadOptionsSize;
	VOID *LoadOptions;
	VOID *ImageBase;
	UINT64 ImageSize;
	EFI_MEMORY_TYPE ImageCodeType;
	EFI_MEMORY_TYPE ImageDataType;
	EFI_IMAGE_UNLOAD Unload;
}EFI_LOADED_IMAGE_PROTOCOL;

struct _EFI_SIMPLE_FILESYSTEM_PROTOCOL;
struct _EFI_FILE_PROTOCOL;


#define EFI_FILE_MODE_CREATE 0x00000000000000000
#define EFI_FILE_MODE_READ 0x00000000000000001
#define EFI_FILE_MODE_WRITE 0x00000000000000002

#define EFI_FILE_READ_ONLY 0x00000000000000001
#define EFI_FILE_HIDDEN 0x00000000000000002
#define EFI_FILE_SYSTEM 0x00000000000000004
#define EFI_FILE_RESERVED 0x00000000000000008
#define EFI_FILE_DIRECTORY 0x00000000000000010
#define EFI_FILE_ARCHIVE 0x00000000000000020
#define EFI_FILE_VALID_ATTR 0x00000000000000037

typedef EFI_STATUS (EFIAPI *EFI_SIMPLE_FILESYSTEM_PROTOCOL_OPEN_VOLUME)
	(IN struct _EFI_SIMPLE_FILESYSTEM_PROTOCOL *This,
	 OUT struct _EFI_FILE_PROTOCOL **Root
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_OPEN)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 OUT struct _EFI_FILE_PROTOCOL **NewHandle,
	 IN CHAR16 *FileName,
	 IN UINT64 OpenMode,
	 IN UINT64 Attributes
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_CLOSE)
	(IN struct _EFI_FILE_PROTOCOL *This
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_DELETE)
	(IN struct _EFI_FILE_PROTOCOL *This
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_READ)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 IN OUT UINTN *BufferSize,
	 OUT VOID *Buffer
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_WRITE)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 IN OUT UINTN *BufferSize,
	 IN VOID *Buffer
);

typedef struct
{
	EFI_EVENT Event;
	EFI_STATUS Status;
	UINTN BufferSize;
	VOID *Buffer;
}EFI_FILE_IO_TOKEN;

typedef EFI_STATUS (EFIAPI *EFI_FILE_OPEN_EX)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 OUT struct _EFI_FILE_PROTOCOL **NewHandle,
	 IN CHAR16 *FileName,
	 IN UINT64 OpenMode,
	 IN UINT64 Attributes,
	 IN OUT EFI_FILE_IO_TOKEN *Token
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_READ_EX)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 IN OUT EFI_FILE_IO_TOKEN *Token
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_WRITE_EX)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 IN OUT EFI_FILE_IO_TOKEN *Token
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_FLUSH)
	(IN struct _EFI_FILE_PROTOCOL *This
);


typedef EFI_STATUS (EFIAPI *EFI_FILE_FLUSH_EX)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 IN OUT EFI_FILE_IO_TOKEN *Token
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_SET_POSITION)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 IN UINT64 Position
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_GET_POSITION)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 OUT UINT64 *Position
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_GET_INFO)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 IN EFI_GUID *InformationType,
	 IN OUT UINTN *BufferSize,
	 OUT VOID *Buffer
);

typedef EFI_STATUS (EFIAPI *EFI_FILE_SET_INFO)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 IN EFI_GUID *InformationType,
	 IN OUT UINTN *BufferSize,
	 IN VOID *Buffer
);

typedef EFI_STATUS (EFIAPI *EFI_DISK_READ)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 IN UINT32 MediaId,
	 IN UINT64 Offset,
	 IN UINTN  BufferSize,
	 OUT VOID  *Buffer
);

typedef EFI_STATUS (EFIAPI *EFI_DISK_WRITE)
	(IN struct _EFI_FILE_PROTOCOL *This,
	 IN UINT32 MediaId,
	 IN UINT64 Offset,
	 IN UINTN  BufferSize,
	 IN VOID   *Buffer
);



typedef struct _EFI_FILE_PROTOCOL
{
	UINT64 Revision;
	EFI_FILE_OPEN Open;
	EFI_FILE_CLOSE Close;
	EFI_FILE_DELETE Delete;
	EFI_FILE_READ Read;
	EFI_FILE_WRITE Write;
	
	EFI_FILE_GET_POSITION GetPosition;
	EFI_FILE_SET_POSITION SetPosition;
	
	EFI_FILE_GET_INFO GetInfo;
	EFI_FILE_SET_INFO SetInfo;
	
	EFI_FILE_FLUSH Flush;
	
	EFI_FILE_OPEN_EX OpenEx;
	EFI_FILE_READ_EX ReadEx;
	EFI_FILE_WRITE_EX WriteEx;
	EFI_FILE_FLUSH_EX FlushEx;
}EFI_FILE_PROTOCOL;

typedef struct _EFI_SIMPLE_FILESYSTEM_PROTOCOL
{
	UINT64 Revision;
	EFI_SIMPLE_FILESYSTEM_PROTOCOL_OPEN_VOLUME OpenVolume;
}EFI_SIMPLE_FILESYSTEM_PROTOCOL;


typedef struct _EFI_FILE_INFO
{
	UINT64 Size;
	UINT64 FileSize;
	UINT64 PhysicalSize;
	
	EFI_TIME CreateTime;
	EFI_TIME LastAccessTime;
	EFI_TIME ModificationTime;
	
	UINT64 Attribute;
	CHAR16 FileName[];
}EFI_FILE_INFO;

typedef struct _EFI_FILE_SYSTEM_INFO
{
	UINT64 Size;
	BOOLEAN ReadOnly;
	UINT64 VolumeSize;
	UINT64 FreeSpace;
	UINT32 BlockSize;
	CHAR16 VolumeLabel[];
}EFI_FILE_SYSTEM_INFO;

typedef struct _EFI_FILE_SYSTEM_VOLUME_LABEL
{
	CHAR VolumeLabel[];
}EFI_FILE_SYSTEM_VOLUME_LABEL;


typedef struct _EFI_DISK_IO_PROTOCOL
{
	UINT64 Revision;
	EFI_DISK_READ ReadDisk;
	EFI_DISK_WRITE WriteDisk;
}EFI_DISK_IO_PROTOCOL;





#endif