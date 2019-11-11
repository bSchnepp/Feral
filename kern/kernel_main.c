/*
Copyright (c) 2018, 2019, Brian Schnepp

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

#include <stdint.h>

#include <feral/feralstatus.h>
#include <feral/stdtypes.h>
#include <feral/handle.h>
#include <feral/kern/frlos.h>
#include <mm/mm.h>

#if defined(__x86_64__)
#include <arch/x86_64/vga/vga.h>
#include <arch/x86_64/cpuio.h>
#include <arch/x86_64/cpufuncs.h>
#endif

#if defined(__x86_64__) || defined(__i386__)
#ifndef FERAL_BUILD_STANDALONE_UEFI_
#include "multiboot/multiboot2.h"
#include <drivers/proc/elf/elf.h>
#endif
#endif

#include <feral/kern/krnlfuncs.h>
#include <feral/kern/krnlbase.h>

#include <krnl.h>
#include <kern_ver.h>

#include <arch/processor.h>

static UINT8 FeralVersionMajor;
static UINT8 FeralVersionMinor;
static UINT8 FeralVersionPatch;

static CHAR* cpu_vendor_msg = "CPU Vendor: ";

/* hack for now */
static UINT64 FreeMemCount;

/* Support up to 8 regions. Hack for now until we get a real malloc. */
static UINT_PTR FreeMemLocs[16];

/* 
	Things to do in order:
	Keyboard input
	In-memory filesystem
	On-disk filesystem (ext2 is enough, 8.3 FAT would be OK to also support)
	Working libc
	System call mechanism. Write a "native" driver (program under env,
	the kernel passes off to the appropriate function for the driver. Kernel
	itself only throws syscall work at drivers to do.)
	We'll eventually need Linux emulation for a "port" of LLVM/Clang, NASM,
	etc., without actually modifying the binary. (Should be unmodified.)
	We'll also want to port a JVM, golang, Python (either 2 or 3), and
	some other stuff too, just for fun.
	
	Work on being self-hosting: Feral needs to compile on Feral.
	We'll also want to create (or port) a nice desktop windowing protocol
	like X, Wayland, or just make a nice one that's specific to Waypoint.
	Likewise, we'll need to figure out how to get some NIC drivers onboard.
	Get rid of multiboot 2 stuff, eventually make our own EFI bootloader.
	Write a new filesystem focusing on getting as good read speeds as
	possible on modern SSDs. At this stage, we'll want support for AHCI,
	SATA, and NVMe. Do not care about IDE/PATA.
	Write a hypervisor, and embed it in the kernel! (This will be fun!)
	Graphics drivers are pretty important to get working.
	
	Nothing older than the vega10 should be supported. There's really no
	need to: I don't have a lot of GPUs lying around, and since
	I'm really only focusing on the hardware I've got, I don't really
	have a reason to support older GPUs, especially from a different vendor.
	(Though, getting Blue GPUs to work sounds fun.)
	
	Getting version control on a Feral system would be great!
	Dogfood, dogfood, and dogfood more. If you don't dogfood, then
	you're not going to be as willing to fix it.
	
	Get a web browser going. Write a LibHTML, possibly port an open
	source ES6 engine like ChakraCore? Build one
	from scratch? At least have HTML4 support, and pretend HTML5 + CSS.
	Also, consider porting Mono.
*/

/* TODO */
#if defined(__aarch64__)
FERALSTATUS KiPrintLine()
{
}

FERALSTATUS KiPrintWarnLine()
{
}
#endif

/* 	
	This is the kernel's *real* entry point. 
	TODO: some mechanism for a Feral-specific bootloader to skip the 
	multiboot stuff and just load this.
*/
VOID KiSystemStartup(KrnlEnvironmentBlock *EnvBlock)
{
	/* First off, ensure we load all the drivers, so we know what's going on. */
	KiPrintLine("Copyright (c) 2018-2019, Brian Schnepp");
	KiPrintLine("Licensed under the Boost Software License.");
	KiPrintFmt("%s\n", "Preparing execution environment...");
	

/* On modern PCs, GOP replaces VGA. We don't need VGA anymore. */
#if defined(__x86_64__) || defined(__i386__) || !defined(FERAL_BUILD_STANDALONE_UEFI_)
	VgaSetCursorEnabled(1);
	VgaTraceCharacters(1);
	VgaMoveCursor(0, 24);
#endif
	
	KiStartupSystem(FERAL_SUBSYSTEM_MEMORY_MANAGEMENT);
	KiStartupSystem(FERAL_SUBSYSTEM_ARCH_SPECIFIC);
	
	/* Only load drivers *after* base system initializtion. */
	KiPrintFmt("Loading all drivers...\n");
	FERALSTATUS KiLoadAllDrivers(VOID);
	
	/* These are macroed away at release builds.  They're eliminated at build time.*/
	KiDebugPrint("INIT Reached here.");
	
	/* 
		TODO: Call up KiStartupProcessor for each processor listed in APIC.
		Each processor should have it's x87 enabled, so we can do SSE stuff
		in usermode. 
	 */
}

//UINT64 because one day someone is going to do something _crazy_ like have an absurd amount of processors (manycore), but be 32-bit and only 4GB addressable.
//RAM is reasonably cheap (less cheap than before) in 2018, so we don't mind using an unneccessary 7 bytes more than we really need to. It's not the 90s where we have to care about a massive 16MB of RAM requirement.
VOID KiStartupProcessor(UINT64 ProcessorNumber)
{
	/*  Create a new stack for this core. */
}



FERALSTATUS KeBootstrapSystem(VOID)
{
	/* Bootstrap process to actually get to user mode. */
	return STATUS_SUCCESS;
}


/* Separate in case needed to implement soft "reboot", ie, just reset OS and RAM contents in memory. */
VOID KiStartupMachineDependent(VOID)
{
#if defined(__x86_64__) || defined(__i386__)
/* Be careful: '__riscv__' is deprecated. */
	x86InitializeIDT();
#elif defined(__riscv)

#elif defined(__aarch64__)

#else
#error Unsupported platform
#endif
}

/* Bring up a system needed for the kernel. */
FERALSTATUS KiStartupSystem(KiSubsystemIdentifier subsystem)
{
	if (subsystem == FERAL_SUBSYSTEM_MEMORY_MANAGEMENT)
	{
		MmPhysicalAllocationInfo allocInfo;
		allocInfo.sType = MM_STRUCTURE_TYPE_PHYSICAL_ALLOCATION_INFO;
		allocInfo.pNext = (void*)(0);
		allocInfo.FrameSize = 4096;
		
		allocInfo.FreeAreaRangeCount = FreeMemCount;
		MmFreeAreaRange ranges[FreeMemCount];
		for (UINT64 i = 0; i < FreeMemCount; ++i)
		{
			ranges[i].sType = MM_STRUCTURE_TYPE_FREE_AREA_RANGE;
			ranges[i].pNext = (void*)(0);
			/* We need in groups of 2. i gives us the group of 2
			   at present.
			 */
			ranges[i].Start = FreeMemLocs[i*2];
			ranges[i].End = FreeMemLocs[(i*2)+1];
			ranges[i].Size = ranges[i].End - ranges[i].Start;
		}
		allocInfo.Ranges = ranges;
		MmCreateInfo info;
		
		info.sType = MM_STRUCTURE_TYPE_MANAGEMENT_CREATE_INFO;
		info.pNext = (void*)(0);
		info.pPhysicalAllocationInfo = &allocInfo;
		return KiInitializeMemMgr(&info);
	} else if (subsystem == FERAL_SUBSYSTEM_ARCH_SPECIFIC) {
		KiStartupMachineDependent();
	} else {
	}
	return STATUS_SUCCESS;
}


/* temporary, turn into clean later. */
VOID InternalPrintRegister(UINT32 reg)
{
	for (int i = 0; i < 4; i++)
	{
		CHAR charToAdd = ((CHAR)(reg >> (i * 8)) & 0xFF);
		KiPutChar(charToAdd);
	}
}

/* ugly hack, refactor sometime later. */
VOID InternalPrintCpuVendor(UINT32 part1, UINT32 part2, UINT32 part3)
{
	KiPrintFmt(cpu_vendor_msg);
	InternalPrintRegister(part1);
	InternalPrintRegister(part2);
	InternalPrintRegister(part3);
	KiPrintLine("");
}

/*
	 We'll need to implement a proper driver for VGA later. 
	 For now, we have something to throw text at and not quickly run out 
	 of space.
 */

#ifndef FERAL_BUILD_STANDALONE_UEFI_
VOID kern_init(UINT32 MBINFO)
{	
	VgaContext graphicsContext = {0};
	UINT8 misc = VgaPrepareEnvironment(&graphicsContext);
	KiBlankVgaScreen(25, 80, VGA_BLACK);
	KiPrintLine("Feral kernel booting...");
	
	FeralVersionMajor = FERAL_VERSION_MAJOR;
	FeralVersionMinor = FERAL_VERSION_MINOR;
	FeralVersionPatch = FERAL_VERSION_PATCH;

	KiPrintFmt("Starting Feral Kernel Version %01u.%01u.%01u %s\n\n", FERAL_VERSION_MAJOR, FERAL_VERSION_MINOR, FERAL_VERSION_PATCH, FERAL_VERSION_SHORT);
	
	/* First, request the info from the multiboot header. */
	if (MBINFO & 0x07)
	{
		/* Unaligned, go panic: todo, clarify it's a multiboot issue. */
		KiStopError(STATUS_ERROR);
	}
	
	/* We need to do some kludgy pointer magic to get this to work. We interpret a pointer as an integer when booting, now need to reinterpret cast to a proper type. */
	/* (We need to treat as an integer initially so that we can check the validity of it: it _must_ be aligned properly. */
	for (multiboot_tag *MultibootInfo = (multiboot_tag*)((UINT64)(MBINFO + 8)); MultibootInfo->type != MULTIBOOT_TAG_TYPE_END; 
	MultibootInfo = (multiboot_tag*)((UINT8*)(MultibootInfo) + ((MultibootInfo->size + 7) & ~0x07)))
	{
		UINT16 type = MultibootInfo->type;
		if (type == MULTIBOOT_TAG_TYPE_CMD_LINE)
		{
			multiboot_tag_string *mb_as_string = (multiboot_tag_string*)(MultibootInfo);
			STRING str = mb_as_string->string;
			UINT64 len = 0;
			if (KiGetStringLength(str, &len) == STATUS_SUCCESS)
			{
				if (len != 0)
				{
					KiPrintFmt("Got command line: %s\n", mb_as_string->string);
				}
			}
			
		} else if (type == MULTIBOOT_TAG_TYPE_BOOT_LOADER) {
			multiboot_tag_string *mb_as_string = (multiboot_tag_string*)(MultibootInfo);
			KiPrint("Detected bootloader: ");
			KiPrintLine(mb_as_string->string);
		} else if (type == MULTIBOOT_TAG_TYPE_BOOT_DEVICE) {
			multiboot_tag_bootdev *mb_as_boot_dev = (multiboot_tag_bootdev*)(MultibootInfo);
			KiPrintFmt("Booted from device %i, slice %i, and partition %i.\nThis will be designated as root (A:/)\n",  mb_as_boot_dev->biosdev, mb_as_boot_dev->slice, mb_as_boot_dev->part);
		} else if (type == MULTIBOOT_TAG_TYPE_MEM_MAP) {
			/* Memory map detected... MB2's kludgy mess here makes this a little painful, but we'll go through this step-by-step.*/
			multiboot_tag_mmap *mb_as_mmap_items = (multiboot_tag_mmap*)(MultibootInfo);
			multiboot_mmap_entry currentEntry = {0};
			
			UINT64 index = 0;
			UINT64 maxIters = (mb_as_mmap_items->size) / mb_as_mmap_items->entry_size;
			
			/* Issue: We add region for every possible area. They're not all free, so we have bigger buffer than needed. */
			FreeMemCount = maxIters;
			UINT64 FreeAreasWritten = 0;
			

			for (currentEntry = mb_as_mmap_items->entries[0]; index < maxIters; currentEntry = mb_as_mmap_items->entries[++index])
			{
				/* Check the type first. */
				if (currentEntry.type == E820_MEMORY_TYPE_FREE)
				{
					/* Write 1: Start pointer */
					FreeMemLocs[FreeAreasWritten] = (UINT_PTR)currentEntry.addr;
					/* Write 2: End pointer */
					FreeMemLocs[FreeAreasWritten+1] = (UINT_PTR)currentEntry.addr + (UINT_PTR)currentEntry.len;
					FreeAreasWritten += 2;
				} else if (currentEntry.type == E820_MEMORY_TYPE_ACPI) {
					KiPrintFmt("ACPI memory at: 0x%x, up to 0x%x\n", currentEntry.addr, currentEntry.addr + currentEntry.len); 
				} else if (currentEntry.type == E820_MEMORY_TYPE_NVS) {
					KiPrintFmt("Reserved hardware memory at: 0x%x, up to 0x%x\n", currentEntry.addr, currentEntry.addr + currentEntry.len); 
				} else if (currentEntry.type == E820_MEMORY_TYPE_BADMEM) {
					KiPrintFmt("DEFECTIVE MEMORY AT: 0x%x, up to 0x%x\n", currentEntry.addr, currentEntry.addr + currentEntry.len);
				}
				
				/* E820_MEMORY_TYPE_RESERVED, E820_MEMORY_TYPE_DISABLED, E820_MEMORY_TYPE_INV ignored. */
			}
			FreeMemCount = FreeAreasWritten >> 1;
			
		} else if (type == MULTIBOOT_TAG_TYPE_ELF_SECTIONS) {
			/* For now, we'll just use the ELF sections tag. */
			multiboot_tag_elf_sections *mb_as_elf = (multiboot_tag_elf_sections*)(MultibootInfo);
			 
			 /* 20 is here because multiboot said so. Don't know why, but that's the size, so go with it. */
			UINT64 maxIters = (mb_as_elf->size - 20) / (mb_as_elf->entsize);
			UINT64 index = 0;
			
			KiPrintFmt("Found %u ELF entries\n", maxIters);
			
			for (UINT64 i = 0; i < maxIters; i++)
			{
				ElfSectionHeader64 *currentEntry = (&mb_as_elf->sections[i * mb_as_elf->entsize]);
				/* todo... */
			}
		}
	}
	
	// We'd like to have some information about the CPU before we boot further.
	// Some things like saying CPU vendor, family, brand name, etc.
	// Eventually, supporting a boot-time flag (and somehow emulating some useful CPU features
	// in-software if not available on the real thing???) would be great.
	// We'll probably use the crypto coprocessor (SHA, etc.) to our advantage with A:/Devices/Hash or something.

	/* This will represent the 4 core registers we need for CPU-specific stuff. */
	UINT32 part1 = 0;
	UINT32 part2 = 0;
	UINT32 part3 = 0;
	UINT32 part4 = 0;

	cpuid_vendor_func(&part1, &part2, &part3);
	InternalPrintCpuVendor(part1, part2, part3);	//TODO: cleanup.

	// 'part1' needs to be a reference to the chunk we want (0x8000000[2, 3, 4])
	// This number is then overridden with the appropriate value for the CPUID brand string.
	// As such, we print IMMEDIATELY, then go and replace part1 with the next bit.
	// We may need to load this into a 12-char buffer later.


	for (int i = 0; i < 3; i++)
	{
		part1 = 0x80000002 + i;
		cpuid_brand_name(&part1, &part2, &part3, &part4);
		InternalPrintRegister(part1);
		InternalPrintRegister(part2);
		InternalPrintRegister(part3);
		InternalPrintRegister(part4);
	}
	KiPrintLine("");	/* Flush to newline. */

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
		/* In both cases, we also need to append some more info from cpuid. */
		actualFamily += ((familyStuff >> 16) & 0x4);
	}
	
	
	if ((actualFamily != CPU_x86_64_FAMILY_ZEN))
	{
		/* 
			Trim down the error messages. TODO: Inspect model number.
			If the model number *is* Zen, but not *first generation 
			Zen*, then also complain about unsupported CPU.
			
			We identify this based on the presence of "1", as in
			1950X, 1700X, 1800X, etc. We must *also* check for "2"
			and "3", since 2000-series are all either Zen+ (supported for sure)
			or zen (also supported for sure.) We have these tight checks
			because inevitably I will make a mistake somewhere and use
			FMA3/RDRAND/etc in a Zen-specific way at some point probably, and won't
			get around to fixing it for a while. Also because the way
			the scheduler is going to be built is *specifically* for
			Zen's chiplet, really-big-cache design. It'll *probably*
			be fine for Zen 2, but I dont have the hardware so I can't
			prove it, even anecdotally. The scheduler is to be built
			with the latency between chiplets in mind, and trying our
			hardest to avoid shuffling between cores, and especially
			between CCXs.
			
			Do note that some APUs ("3000-series") are actually Zen+,
			not Zen 2. These are identified with the suffix "GE" and "G".
			
			Then we have to deal with the embedded CPUs. (V, R-series).
			Both of these are Zen 1.
		*/
		KiPrintLine("Unsupported CPU");
	} 
	if (TRUE)
	{
		/* 
			Put this here until we get B350, X370, etc. drivers.
			Feral (for now) doesn't support _any_ platform controller
			hub, so this message is pretty much meaningless.
			
			We need to actually write code to identify between
			X370, X470, X570, B350, B450, B550, B320, B420, and B520.
			(oh and panther point and wellsburg or whatever for
			the blue team if we care.)
			
			We'll need these for doing chipset-specific stuff
			one day, like telling the x570 to do RAID, or
			nicely asking about USB devices connected to it and
			not the CPU directly. (over PCI, of course.)
		 */
		KiPrintLine("Unsupported PCH");
	}

	KrnlEnvironmentBlock EnvBlock;
	/* Kernel initialization is done, move on to actual tasks. */
	KiSystemStartup(&EnvBlock);
}
#endif
