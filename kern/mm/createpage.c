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
 
#include <feral/feralstatus.h>
#include <feral/stdtypes.h>

#include <mm/mm.h>
#include <mm/page.h>

#if defined(__x86_64__) || defined(__i386__)
/* TODO: Deal with 32-bit x86 not supporting everything. Maybe force PAE? */
#include <arch/x86_64/mm/pageflags.h>
#endif

typedef enum MmStructureType
{
	MM_STRUCTURE_TYPE_FRAME_ALLOCATION_CONTEXT = 0,
	MM_STRUCTURE_TYPE_PAGE_DIRECTORY,
	MM_STRUCTURE_TYPE_MAX = 0xFFFF
}MmStructureType;

/* An entry in the page table. */
typedef uint64_t PageTableEntry;

typedef struct MmPageDirectory
{
	MmStructureType sType;
	const char* pNext;
	
	PageTableEntry	*Tables[1024];
	UINT_PTR PhysicalAddresses[1024];
	UINT_PTR PhysicalTableRealAddress;
}MmPageDirectory;

typedef struct MmFrameAllocationContext
{
	MmStructureType sType;
	const void *pNext;
	
	UINT8 *PageMap;
	UINTN PageMapSize;
	
	UINT8 *HeapMap;
	UINTN HeapSize;
	
	UINT_PTR PageTable;
	
	UINT_PTR StartingAddress;
	UINT_PTR *ForbiddenAddresses;
	UINTN ForbiddenAddressedCount;
}MmFrameAllocationContext;




/* We'll allow processes up to ~140TB of virtual memory. This should be more than enough for the time being. 
 	Maybe we can be *really* aggressive and only allow 1TB or less of virtual memory for the kernel?
 	We'll probably have no need for something even that massive for the time being, (realistically,
 	if the kernel needs more than 4GB of RAM, something is very wrong.)
 	
 	
 	What needs to happen is we create a bare-bones malloc here,
 	with the presumption we have something like at least 64MB of RAM
 	(which is pretty reasonable, actually, and we use it to bootstrap
 	a new GDT so we eventually get to higher half. (along with
 	a physical memory manager)
 	
 	When that happens, we *reinitialize* this same allocator, and
 	use it for a now permanent kernel heap.
 */

/* TODO: Make this cleaner... */
static MmFrameAllocationContext CurrentAllocationContext;

/* These need to be moved out, and just here for now. */
FERALSTATUS KiInitializeMemMgr(MemoryManagementCreateInfo info, UINT8 *HeapArea)
{
#if defined(__x86_64__) || defined(__i386__)
	CurrentAllocationContext.PageTable = x86_read_cr3();
#endif
	CurrentAllocationContext.HeapMap = HeapArea;
	/* Do we need this...? */
	CurrentAllocationContext.PageMapSize = info.PageAllocSize;
	/* Trim last 4 hex vals, so it becomes aligned. 256 is minimum size for malloc. */
	UINT64 AreaAligned = ((UINT64)(info.HeapSize)) & 0xFFFFFFFFFFFF0000;
	KiPrintFmt("Heap size is actually %u bytes big \n", AreaAligned);
	
	/* Which means we need an area of that size, but divided by 256. */
	UINT64 HeapMapSize = AreaAligned / 256;
	/* And an area to put it at... Add 4096 since we should keep a page in 
	to block buffer overruns (todo: implement that). We need to check we 
	actually have this much RAM though. */
	CurrentAllocationContext.HeapMap = (UINTN*)(HeapArea + AreaAligned + 4096);
	KiPrintFmt("Heap chunk area is at: 0x%x\n", CurrentAllocationContext.HeapMap);
	return STATUS_SUCCESS;
}


FERALSTATUS MmCreatePageTables(VOID)
{
	/* On x86, we need to flush the CR3 and update it.
	   This will cause a TLB flush.
	 */
	return STATUS_ERROR;
}

FERALSTATUS MmAllocateProcess(VOID)
{
	return STATUS_ERROR;
}



/* 
	Likewise, here temporarilly, need to be moved out.
	In the future, we want to replace all of this
	with a buddy memory system (the same as what Linux usses.)
	I'm mostly interested in filesystem and scheduling,
	we just need a malloc() to make everything else possible
	first.
 */
 
/**
	Prepares a given location to be allocated by MmAllocateFrame.
	This simply marks a single given page as in use in the page map,
	and then returns the associated address in AllocatedLocation.
 */
FERALSTATUS MmPreAllocateFrame(IN UINTN MaxPages, IN MmFrameAllocationContext AllocationContext, OUT UINT_PTR *AllocatedLocation)
{
	if (AllocationContext.sType != MM_STRUCTURE_TYPE_FRAME_ALLOCATION_CONTEXT)
	{
		/* Something got passed in very wrong. */
		return STATUS_ERROR;
	}
	
	UINTN Counter = 0;
	while (AllocationContext.PageMap[Counter] != 0x00)
	{
		if (++Counter == MaxPages)
		{
			/*  TODO: Add error in FERALSTATUS*/
			return STATUS_ERROR;
		}
	}
	AllocationContext.PageMap[Counter] = 0x01;
	return AllocationContext.StartingAddress + (0x1000 * Counter);
}

FERALSTATUS MmAllocateFrame(IN UINTN MaxPages, IN MmFrameAllocationContext AllocationContext, OUT UINT_PTR *AllocatedLocation)
{
	/* TODO */
	return STATUS_SUCCESS;
}


FERALSTATUS MmGetContainingFrame(IN UINT_PTR Address, OUT MmPage *Page)
{
	return STATUS_SUCCESS;	
}


FERALSTATUS MmAllocateMemory(IN UINT_PTR RequestedAddress, UINTN Size, INOPT UINT_PTR LowerBound, INOPT UINT_PTR UpperBound, OUT UINT_PTR *ActualAddress)
{
	return STATUS_SUCCESS;	
}


/**
	Allocates a given page with given size and at the requested address.
	If the specific address with the specific size cannot be allocated,
	then an error status is returned.
	
	SpecificAddress will be set to the actual location in memory where
	the needed space was allocated.
 */
FERALSTATUS MmAllocateSpecificMemory(INOUT UINT_PTR SpecificAddress, UINTN Size)
{
	return STATUS_SUCCESS;	
}

/**
	Marks a given page as empty.
	@param AddressLocation The location to free
	@param Size The amount to free
	
	@return The status of the kernel at this given time.
 */
FERALSTATUS MmDeallocateMemory(IN UINT_PTR AddressLocation, UINTN Size)
{
	return STATUS_SUCCESS;
}



FERALSTATUS MmCreatePage(BOOL HugePage, UINT8 PageLevel, UINT64 NumPages, UINT8 RingLevel)
{
#if defined(__x86_64__) || defined(__i386__)
	PageTableEntry entry = X86_PRESENT_WRITABLE_PAGE;
	if (HugePage)
	{
		entry |= X86_PAGE_HUGE;  
	}
#endif
	return STATUS_SUCCESS;
}

