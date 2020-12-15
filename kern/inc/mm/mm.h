/*
Copyright (c) 2018, 2019, Brian Schnepp

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

#include <feral/stdtypes.h>
#include <feral/feralstatus.h>

#include <mm/heap.h>
#include <mm/mmcommon.h>

#ifndef _FERAL_KERN_MM_H_
#define _FERAL_KERN_MM_H_

typedef enum MmStructureType
{
	MM_STRUCTURE_TYPE_MANAGEMENT_CREATE_INFO = 0,

	MM_STRUCTURE_TYPE_PHYSICAL_ALLOCATION_INFO = 1,
	MM_STRUCTURE_TYPE_PHYSICAL_FRAME_ALLOCATOR = 2,
	MM_STRUCTURE_TYPE_PHYSICAL_FRAME_BLOCK = 3,

	MM_STRUCTURE_TYPE_VIRTUAL_ALLOCATION_INFO = 4,
	MM_STRUCTURE_TYPE_VIRTUAL_FRAME_ALLOCATOR = 5,
	MM_STRUCTURE_TYPE_VIRTUAL_FRAME_BLOCK = 6,

	MM_STRUCTURE_TYPE_HEAP_ALLOCATION_INFO = 7,
	MM_STRUCTURE_TYPE_HEAP_ALLOCATOR = 8,

	MM_STRUCTURE_TYPE_FREE_AREA_RANGE = 9,
	MM_STRUCTURE_TYPE_USED_AREA_RANGE = 10,
	MM_STRUCTURE_TYPE_RESERVED_AREA_RANGE = 11,
	MM_STRUCTURE_TYPE_OTHER_AREA_RANGE = 12,

	MM_STRUCTURE_TYPE_MAX = 0xFFFF
} MmStructureType;

typedef struct MmMemoryRange
{
	MmStructureType SType;
	void *PNext;

	UINT_PTR Start;
	UINT_PTR End;
	UINT_PTR Size; /* Not *actually* a pointer, but matches addr size. */
} MmMemoryRange;


typedef struct MmPhysicalAllocationInfo
{
	MmStructureType sType;
	void *pNext;

	UINT64 FrameSize; /*Expect 4096 for now. Possibly use 2MB pages?*/
	UINT64 MemoryAreaRangeCount;
	MmMemoryRange *Ranges;
} MmPhysicalAllocationInfo;


typedef struct MmCreateInfo
{
	MmStructureType sType;
	void *pNext;

	MmPhysicalAllocationInfo *pPhysicalAllocationInfo;

	VOID* SafePMMArea;
} MmCreateInfo;

typedef struct AllocatorState
{
	MmStructureType sType;
	void *pNext;

	UINT64 NumArenas;
	Arena *Arenas;
} AllocatorState;


/* (Obviously, these are TODO.) FIXME: Prefix with 'Mm'. */

FERALSTATUS KiInitializeMemMgr(IN MmCreateInfo *info); /* TODO!!! */

FERALSTATUS GetMemoryAlreadyInUse(IN UINT_PTR Location, OUT BOOL *Status);
FERALSTATUS SetMemoryAlreadyInUse(IN UINT_PTR Location, IN BOOL Status);

FERALSTATUS ExtractAddressFromPageEntry(
	IN PageMapEntry *Entry, OUT UINT_PTR *Address);



AllocatorState *MmCreateAllocatorState(
	UINT64 NumArenas, VOID *HeapArea, UINT_PTR HeapSize);

void *MmKernelMalloc(UINT64 Size);

void MmKernelFree(UINT_PTR Location);


typedef enum KernMemProtect
{
	KERN_MEM_PROTECT_ALLOW_READ = 0,
	KERN_MEM_PROTECT_ALLOW_WRITE = 1,
	KERN_MEM_PROTECT_ALLOW_EXECUTE = 2,
}KernMemProtect;

typedef enum KernMemUsage
{
	KERN_MEM_USAGE_RESERVE = 0,
	KERN_MEM_USAGE_SWAPPABLE = 1,
	KERN_MEM_USAGE_DMA = 2,
	KERN_MEM_USAGE_LARGE_PAGES = 4,
	KERN_MEM_USAGE_VERY_LARGE_PAGES = 8,
	KERN_MEM_USAGE_NO_CACHE = 16,

}KernMemUsage;


FERALSTATUS MmKernelAllocPages(IN UINT64 Pages, 
	IN KernMemProtect Protection, 
	IN KernMemUsage Usage, 
	IN PageMapEntry *PML4,
	INOPT VOID *BaseAddr,
	INOPT VOID *BaseVAddr, 
	OUT VOID **Result);

FERALSTATUS MmKernelDeallocPages(IN UINT64 Pages, IN VOID *Address);

#endif
