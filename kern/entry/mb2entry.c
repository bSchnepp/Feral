/*
Copyright (c) 2020 Brian Schnepp

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
#include <stdint.h>

#if defined(__x86_64__)
#include <arch/x86_64/cpuio.h>
#include <arch/x86_64/cpuinfo.h>
#include <arch/x86_64/cpufuncs.h>
#endif

/* Borrow a bunch from Feralboot. */
#ifndef _NO_UEFI_STUB_
#include <libreefi/efi.h>
#endif

#include <feral/feralstatus.h>
#include <feral/stdtypes.h>
#include <feral/handle.h>
#include <feral/kern/frlos.h>
#include <mm/mm.h>

#include <feral/kern/krnlfuncs.h>
#include <feral/kern/krnlbase.h>

#include <krnl.h>
#include <kern_ver.h>


/* hack: include the serial driver in a brute forcey way. */
#include <drivers/serial/serial.h>


/* hack for now */
static UINT64 FreeMemCount;

/* Support up to 8 regions. Hack for now until we get a real malloc. */
static UINT_PTR FreeMemLocs[16];

/*
	 We'll need to implement a proper driver for VGA later.
	 For now, we have something to throw text at and not quickly run out
	 of space.
 */

#ifndef FERAL_BUILD_STANDALONE_UEFI_
#include "multiboot/multiboot2.h"
#include <drivers/proc/elf/elf.h>
#include <arch/x86_64/vga/vga.h>

static CHAR *cpu_vendor_msg = "CPU Vendor: ";


/* temporary, turn into clean later. */
VOID InternalPrintRegister(UINT32 reg)
{
	for (int i = 0; i < 4; i++)
	{
		CHAR charToAdd = ((CHAR)(reg >> (i * 8)) & 0xFF);
		VgaAutoEntry(VGA_GREEN, VGA_BLACK, charToAdd);
	}
}

/* ugly hack, refactor sometime later. */
VOID InternalPrintCpuVendor(UINT32 part1, UINT32 part2, UINT32 part3)
{
	VgaAutoPrint(VGA_GREEN, VGA_BLACK, cpu_vendor_msg);
	InternalPrintRegister(part1);
	InternalPrintRegister(part2);
	InternalPrintRegister(part3);
	VgaAutoPrintln(VGA_WHITE, VGA_BLACK, "");
}

static KrnlFirmwareFunctions FirmwareFuncs = {0};
static KrnlCharMap CharMap = {0};
static KrnlEnvironmentBlock EnvBlock = {0};

static STRING GetBiosFirmwareClaim();
static VOID InternalVgaPrintln(STRING Str, UINT64 Len);
static VOID InternalVgaBackspace();

static STRING GetBiosFirmwareClaim()
{
	return "PC Compatible BIOS (Multiboot 2)";
}

static UINT_PTR MaxAddress = 0x00;
static UINT_PTR(InternalGetMaxPhysicalAddressFunc)(VOID)
{
	return MaxAddress;
}
static VOID InternalVgaPrintln(STRING Str, UINT64 Len)
{
	VgaPrintln(VGA_WHITE, VGA_BLACK, Str, Len);
}

static VOID InternalVgaBackspace()
{
	UINT16 X = 0;
	UINT16 Y = 0;
	VgaGetCurrentPosition(&X, &Y);

	UINT16 Width = 0;
	UINT16 Height = 0;
	VgaGetFramebufferDimensions(&Width, &Height);

	if (X == 0)
	{
		return;
	}
	--X;

	VgaMoveCursor(X, Y);
	VgaPutChar(' ');
	VgaMoveCursor(X, Y);
}

VOID kern_init(UINT32 MBINFO)
{
	UINT8 misc = VgaPrepareEnvironment();
	KiBlankVgaScreen(25, 80, VGA_BLACK);
	VgaAutoPrintln(
		VGA_WHITE, VGA_BLACK, "Starting initial kernel setup...");
	/* First, request the info from the multiboot header. */
	if (MBINFO & 0x07)
	{
		/*
		 * If the alignment was wrong, that means that
		 * this either isn't multiboot, there is a memory issue,
		 * or that the bootloader doesn't implement the specification
		 * correctly. In any case, we should immediately
		 * kernel panic and prevent any further loading.
		 */
		KiStopError(STATUS_ERROR);
	}
	/*
	 * This is absolutely terrible code that needs to happen
	 * in order to make multiboot work right.
	 *
	 * First, force cast the argument to multiboot info one entry past where
	 * the bootloader told us to go.
	 *
	 * Then, some info needs to get queried from it, like each of it's tags.
	 * To check the tags, the real pointer magic begins by taking
	 * an offset by adding the size of the items which are in the way
	 * to the base pointer, force casing this, and looping until the
	 * end tag is reached.
	 *
	 * Each of these tags is then processed based on the ID of it.
	 */
	uint64_t MaxMem = 0;
	for (multiboot_tag *MultibootInfo
		= (multiboot_tag *)((UINT_PTR)(MBINFO + 8));
		MultibootInfo->type != MULTIBOOT_TAG_TYPE_END;
		MultibootInfo
		= (multiboot_tag *)((UINT8 *)(MultibootInfo)
				    + ((MultibootInfo->size + 7) & ~0x07)))
	{
		UINT16 Type = MultibootInfo->type;
		if (Type == MULTIBOOT_TAG_TYPE_CMD_LINE)
		{
			multiboot_tag_string *mb_as_string
				= (multiboot_tag_string *)(MultibootInfo);
			STRING str = mb_as_string->string;
			UINT64 len = 0;
			if (KiGetStringLength(str, &len) == STATUS_SUCCESS)
			{
				if (len != 0)
				{
					VgaAutoPrint(VGA_WHITE, VGA_BLACK,
						"Got command line: ");
					VgaAutoPrintln(VGA_RED, VGA_BLACK,
						mb_as_string->string);
				}
			}
		}
		else if (Type == MULTIBOOT_TAG_TYPE_BOOT_LOADER)
		{
			multiboot_tag_string *mb_as_string
				= (multiboot_tag_string *)(MultibootInfo);
			VgaAutoPrint(
				VGA_WHITE, VGA_BLACK, "Detected bootloader: ");
			VgaAutoPrintln(VGA_LIGHT_BROWN, VGA_BLACK,
				mb_as_string->string);
		}
		else if (Type == MULTIBOOT_TAG_TYPE_BOOT_DEVICE)
		{
			multiboot_tag_bootdev *mb_as_boot_dev
				= (multiboot_tag_bootdev *)(MultibootInfo);
			/* Care about this stuff later... */
		}
		else if (Type == MULTIBOOT_TAG_TYPE_MEM_MAP)
		{
			/* Memory map detected... MB2's kludgy mess here makes
			 * this a little painful, but we'll go through this
			 * step-by-step.*/
			multiboot_tag_mmap *mb_as_mmap_items
				= (multiboot_tag_mmap *)(MultibootInfo);
			multiboot_mmap_entry currentEntry = {0};

			UINT64 index = 0;
			UINT64 maxIters = (mb_as_mmap_items->size)
					  / mb_as_mmap_items->entry_size;

			/* Issue: We add region for every possible area. They're
			 * not all free, so we have bigger buffer than needed.
			 */
			FreeMemCount = maxIters;
			UINT64 FreeAreasWritten = 0;

			/* Go through all the entries and try to see what type
			 * it is. */
			for (currentEntry = mb_as_mmap_items->entries[0];
				index < maxIters;
				currentEntry
				= mb_as_mmap_items->entries[++index])
			{
				/* Check the Type first. */
				if (currentEntry.type == E820_MEMORY_TYPE_FREE)
				{
					/* Write 1: Start pointer */
					FreeMemLocs[FreeAreasWritten]
						= (UINT_PTR)currentEntry.addr;
					/* Write 2: End pointer */
					FreeMemLocs[FreeAreasWritten + 1]
						= (UINT_PTR)currentEntry.addr
						  + (UINT_PTR)currentEntry.len;
					FreeAreasWritten += 2;
				}
				else if (currentEntry.type
					 == E820_MEMORY_TYPE_ACPI)
				{
					/* 16 chars will fit the whole address*/
					CHAR BufBeginAddr[17];
					CHAR BufEndAddr[17];
					internalItoaBaseChange(
						currentEntry.addr, BufBeginAddr,
						16);
					internalItoaBaseChange(
						currentEntry.addr
							+ currentEntry.len,
						BufBeginAddr, 16);
					VgaAutoPrint(VGA_WHITE, VGA_BLACK,
						"ACPI memory at: 0x");
					VgaAutoPrint(VGA_GREEN, VGA_BLACK,
						BufBeginAddr);
					VgaAutoPrint(VGA_WHITE, VGA_BLACK,
						", up to 0x");
					VgaAutoPrintln(
						VGA_RED, VGA_BLACK, BufEndAddr);
				}
				else if (currentEntry.type
					 == E820_MEMORY_TYPE_NVS)
				{
					/* 16 chars will fit the whole address*/
					CHAR BufBeginAddr[17];
					CHAR BufEndAddr[17];
					internalItoaBaseChange(
						currentEntry.addr, BufBeginAddr,
						16);
					internalItoaBaseChange(
						currentEntry.addr
							+ currentEntry.len,
						BufBeginAddr, 16);
					VgaAutoPrint(VGA_WHITE, VGA_BLACK,
						"Reserved hardware memory at: "
						"0x");
					VgaAutoPrint(VGA_GREEN, VGA_BLACK,
						BufBeginAddr);
					VgaAutoPrint(VGA_WHITE, VGA_BLACK,
						", up to 0x");
					VgaAutoPrintln(
						VGA_RED, VGA_BLACK, BufEndAddr);
				}
				else if (currentEntry.type
					 == E820_MEMORY_TYPE_BADMEM)
				{
					/* 16 chars will fit the whole address*/
					CHAR BufBeginAddr[17];
					CHAR BufEndAddr[17];
					internalItoaBaseChange(
						currentEntry.addr, BufBeginAddr,
						16);
					internalItoaBaseChange(
						currentEntry.addr
							+ currentEntry.len,
						BufBeginAddr, 16);
					VgaAutoPrint(VGA_WHITE, VGA_BLACK,
						"BAD MEMORY at: 0x");
					VgaAutoPrint(VGA_GREEN, VGA_BLACK,
						BufBeginAddr);
					VgaAutoPrint(VGA_WHITE, VGA_BLACK,
						", up to 0x");
					VgaAutoPrintln(
						VGA_RED, VGA_BLACK, BufEndAddr);
				}

				/* E820_MEMORY_TYPE_RESERVED,
				 * E820_MEMORY_TYPE_DISABLED,
				 * E820_MEMORY_TYPE_INV ignored. */
			}
			FreeMemCount = FreeAreasWritten >> 1;
			for (uint32_t Index = 0; Index < FreeAreasWritten;
				++Index)
			{
				if (FreeMemLocs[Index] > MaxAddress)
				{
					MaxAddress = FreeMemLocs[Index];
				}
			}
		}
		else if (Type == MULTIBOOT_TAG_TYPE_ELF_SECTIONS)
		{
			/* For now, we'll just use the ELF sections tag. */
			multiboot_tag_elf_sections *mb_as_elf
				= (multiboot_tag_elf_sections *)(MultibootInfo);

			/* 20 is here because multiboot said so. Don't know why,
			 * but that's the size, so go with it. */
			UINT64 maxIters
				= (mb_as_elf->size - 20) / (mb_as_elf->entsize);
			UINT64 index = 0;

			/* Don't care here... */

			for (UINT64 i = 0; i < maxIters; i++)
			{
				ElfSectionHeader64 *currentEntry
					= (&mb_as_elf->sections
							[i * mb_as_elf->entsize]);
				/* todo... */
			}
		}
	}

	/* This will represent the 4 core registers we need for CPUID stuff. */
	UINT32 part1 = 0;
	UINT32 part2 = 0;
	UINT32 part3 = 0;
	UINT32 part4 = 0;

	/* Some inline asm magic is used to get the CPU vendor.
	 * (ie, "KVM KVM KVM"). Thus, we use 3 dwords to pretend to
	 * be some scratch registers and then reinterpret it as a 12-byte
	 * string.
	 */
	cpuid_vendor_func(&part1, &part2, &part3);
	InternalPrintCpuVendor(part1, part2, part3);

	/* To get the reported name of the CPU,
	 * we'll need 48 bytes. On the x86, this is done
	 * by calling cpuid with the command (0x80000002 + i),
	 * where i is the segment of 16 bytes to get. We then do
	 * the same thing as with the CPU vendor thing, and reinterpret
	 * the 4 fake scratch registers as a string of 16 bytes, and then
	 * do this until all 48 characters are printed.
	 */
	UINT_PTR CpuNameCommand = 0x80000002;
	for (UINT_PTR Index = 0; Index < 3; ++Index)
	{
		part1 = CpuNameCommand + Index;
		cpuid_brand_name(&part1, &part2, &part3, &part4);
		InternalPrintRegister(part1);
		InternalPrintRegister(part2);
		InternalPrintRegister(part3);
		InternalPrintRegister(part4);
	}
	/* When we're done with CPUID, insert a newline. */
	VgaAutoPrintln(VGA_WHITE, VGA_BLACK, "");

	UINT32 familyStuff = cpuid_family_number();
	UINT32 actualFamily = (familyStuff >> 8) & 15;
	UINT32 extendedModel = (familyStuff >> 16) & (0xF);
	UINT32 baseModel = (familyStuff >> 4) & (0xF);
	UINT32 actualModel = baseModel + (extendedModel << 4);

	if (actualFamily == 0x6 || actualFamily == 0xF)
	{
		/* Family 15 wants us to report the family number as such. */
		if (actualFamily == 15)
		{
			actualFamily += (familyStuff >> 20) & 0x0FFFFFFF;
		}
		/* In both cases, we also need to append some more info from
		 * cpuid. */
		actualFamily += ((familyStuff >> 16) & 0x4);
	}


	if ((actualFamily != CPU_x86_64_FAMILY_ZEN))
	{
		/* At the moment, I am only testing on Zen hardware.
		 * anything else is unsupported (but will probably work
		 * just fine.)
		 */
		VgaAutoPrintln(VGA_RED, VGA_BLACK, "Unsupported CPU");
	}

	/* We'll want the cursor to always follow us! */
	VgaSetCursorEnabled(TRUE);
	VgaTraceCharacters(TRUE);
	VgaMoveCursor(0, 24);

	/* To generalize the kernel's boot process, we should
	 * define some functions in a sort of "interface" to the rest of
	 * the kernel now that we're about to leave the bootloader-specific
	 * block.
	 *
	 * When that happens, we'll of course want a way to put things on
	 * the screen, so let the rest of the kernel know how to do that, along
	 * with some other things.
	 */
	FirmwareFuncs.PutChar = VgaPutChar;
	FirmwareFuncs.Println = InternalVgaPrintln;
	FirmwareFuncs.GetFirmwareName = GetBiosFirmwareClaim;
	FirmwareFuncs.Backspace = InternalVgaBackspace;
	FirmwareFuncs.GetMaxPhysicalAddress = InternalGetMaxPhysicalAddressFunc;
	EnvBlock.FunctionTable = &FirmwareFuncs;
	EnvBlock.CharMap = &CharMap;
	/* Kernel initialization is done, move on to actual tasks. */
	KiSystemStartup(&EnvBlock);
}

/* Bring up a system needed for the kernel. */
FERALSTATUS KiStartupSystem(KiSubsystemIdentifier subsystem)
{
	/* This defines where free memory is and isn't,
	 * at the start of boot time. We assume users don't
	 * do anything silly like hotplug memory, and if it's
	 * valid memory at boot time, it's always valid memory.
	 *
	 * We also assume a fairly sane memory layout that doesn't
	 * have a bunch of fragmentation. If there's too much fragmentation,
	 * we just ignore the extra memory. That is a problem for now, but
	 * something we can fix later if it's serious enough.
	 */
	if (subsystem == FERAL_SUBSYSTEM_MEMORY_MANAGEMENT)
	{
		MmPhysicalAllocationInfo allocInfo;
		allocInfo.sType = MM_STRUCTURE_TYPE_PHYSICAL_ALLOCATION_INFO;
		allocInfo.pNext = (void *)(0);
		allocInfo.FrameSize = 4096;

		allocInfo.FreeAreaRangeCount = FreeMemCount;
		MmFreeAreaRange ranges[FreeMemCount];
		for (UINT64 i = 0; i < FreeMemCount; ++i)
		{
			ranges[i].sType = MM_STRUCTURE_TYPE_FREE_AREA_RANGE;
			ranges[i].pNext = (void *)(0);
			/* We need in groups of 2. i gives us the group of 2
			   at present.
			 */
			ranges[i].Start = FreeMemLocs[i * 2];
			ranges[i].End = FreeMemLocs[(i * 2) + 1];
			ranges[i].Size = ranges[i].End - ranges[i].Start;
		}
		allocInfo.Ranges = ranges;
		MmCreateInfo info;

		info.sType = MM_STRUCTURE_TYPE_MANAGEMENT_CREATE_INFO;
		info.pNext = NULLPTR;
		info.pPhysicalAllocationInfo = &allocInfo;
		FERALSTATUS Status = KiInitializeMemMgr(&info);
		if (Status != STATUS_SUCCESS)
		{
			return Status;
		}

		/* First part of manipulating page tables in C... 
		 * We no longer need lower half, so just wipe it out.
		 */
		UINT_PTR *OrigP4 = get_initial_p4_table();
		//OrigP4[0] = 0x00;
		//x86_write_cr3(OrigP4);
		return STATUS_SUCCESS;
	}
	else if (subsystem == FERAL_SUBSYSTEM_ARCH_SPECIFIC)
	{
		/* Machine-dependent setup, for things like
		 * serial drivers, higher resolution VGA, etc.
		 */

		KiStartupMachineDependent();
		/* TODO: Enable serial driver
		 * iff use-serial=true, instead of
		 * whenever there is the
		 * command line...
		 */
		VOID *Databack = NULLPTR;
		InitSerialDevice(Databack);
	}
	else
	{
		/*  Placeholder for more stuff later on. (disks, network...) */
	}
	return STATUS_SUCCESS;
}

#endif
