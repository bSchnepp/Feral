# FERAL KERNEL

## What is Feral?
Feral is an experimental monolithic kernel, which is intended for use by my Waypoint operating system. It's primary goals
are flexibility, compatibility, security, and performance, roughly in that order.

Feral is designed to run on recent PCs, as of 2020, with the core architecture belonging to either the "Skylake" or
direct descendants such as "Kaby Lake", "Ice Lake", and "Coffee Lake", as well as the "Zen" microarchitecture, and it's
immediate descendants, such as "Zen+", "Zen 2", and "Zen 3", where the firmware is either compatible with BIOS, as a
traditional PC, or is compatible with UEFI version 2.8. As such, certain x86 systems like "Liverpool" or "Scorpio" are
not supported, as well as legacy x86 architectures such as i486 and i686.

## Supported Hardware

Feral, for now, assumes the presence of the two 8259A PICs present in traditional
PC-compatible microcomputers. On modern micros, your southbridge, such as x399 or x470,
probably has something compatible. In the future, Feral intends to move on to
utilization of newer processor features, such as the APIC, and eventually the 
xAPIC and x2APIC. In addition, Feral will support symmetric multiprocessing.
This however, does mean that for certain kinds of hardware with very strange
BIOSes, such as the aforementioned Liverpool CPU, it may be difficult or even
impossible for Feral to ever run on such a device.

Feral is in the process of a rewrite to some core initialization functions, partly to allow porting to devices
which are truely headless, partly to allow for early setup of video output protocols, and to deprecate Multiboot 2 support.
For now, Feral is most functional when set up to boot over Multiboot 2 protocol. The UEFI port is incomplete, and
does not support the timers, or outputs to video after loading of the kernel.

Currently, hardware being tested includes a PC with a 4"x4" motherboard and an N3700 CPU ("Braswell") with 4GB of RAM,
a 17z laptop with a 2500U (aforementioned Zen 1), and in the future, an x5-Z8350 with 2GB of RAM ("Cherry Trail").

In the future, Feral is intended to run on Aarch64 (BCM2711), POWER (Sforza uarch), and RV64GC (U540 SoC) hardware,
with ports written somewhere in that order.

## Core architecture?
Architecturally, Feral is a simple monolithic kernel, which architecturally shares
many similaries with Mach and Plan 9, especially the latter. Feral prefers to place
most functionality into drivers, rather than baked into the kernel itself, to allow
for more flexibility, and to expose resources as a network location and socket, to
better reflect how modern processes and storage work.

For example, to open  a file, one queries the file server, which is a process
owned by the kernel, which opens a socket which accepts incoming traffic.
It can then be read from, or written to, depending on how the socket was
opened.

## Why Feral?

Feral is partly created out of boredom of "yet another *NIX clone". There isn't anything inherently wrong with that architecture,
but it's been done a lot and it's always more or less the same, without really exploring any ideas outside of that narrow "everything is a file"
approach.

Another part is "how 'alien' can an operating system be from *NIX, while still being fairly simple to use, port software to, and having a clean design"?
Feral tries to get as far away from "everything is a file" as possible, instead exposing "everything is a network resource". This is useful for
some cases where some component may not make sense as being exposed as a file, such as the temperture indicator of a video card. It wouldn't make
any sense to be able to write to that. Rather than making it complicated with writes denied for everyone (including root, who should be able to do anything!), 
Feral opts to just expose something like that as a network resource rejecting any inbound traffic, but allowing incoming connections to read it's data.

This, conceptually, allows any component to expose itself as possible to read, possible to write, have stateless or statefulness, and
allows it to be streamed over a network (in a cluster configuration), allowing scalability horizontally.

Feral is best described as a "modular macrokernel", or just a "monolithic kernel with loadable drivers at runtime", which is a little bit harder to say.

![Feral Architecture](Documentation/images/feralarch.png)

## Building?
Run ./build.sh $ARCH $COMMAND,
where $ARCH is the arch (x86_64), and $COMMAND
is the command to run (qemu, qemu-nokvm, clean, etc.)

This is temporary, and will be replaced with a proper
build system (Kconfig) later.

## Minimum requirements?
Feral should run comfortably on any PC implementing the x86-64 instruction set
(ie, at least K8 CPU), and at least 1GB of system memory. This memory requirement 
is done out of laziness in the assembly bootstrapping stage, and Feral is expected
to lower system requirements to 128MB of RAM in the future.

Feral expects a VGA-compatible display adapter to be present for a PC. As such,
you'll need some form of video adapter capable of running in VGA mode. Most
consumer-class CPUs and GPUs implement this.

Feral is tested on a 1950X processor through KVM, and is only *supported* running on version
1 of the Zen microarchitecture. Issues due to a lack of instruction set support,
misrepresented FMA4 support, etc. from another uarch may not be patched immediately.



## Is there any naming convention?
Folders should be named in a way to avoid unnecessary characters (ie, 'inc' vs 'include'.)
Likewise, for a given module, the folder name should reflect the prefix, such that
memory management is in "mm", and object management is in "ob", and so on.

Functions prefixed with "internal" are for application-specific functions which are simply helper routines, or even temporary ones.
They are typically not defined in a header file, and should not be used outside of the source file they are present in.
```c
VOID internalDoSomething()
{
	return;
}

internalDoSomething();
```

I've broken this rule a few times, but this should be fixed as soon as possible.

Functions intended to be stable should use:

	- Minimal characters prefixed to define what it does ('Hal', 'Vd', 'Ke', 'Vga', 'Sd', etc.)

Kernel-related functions should ALWAYS
	
	- Insist on using arguments as inputs/outputs, using the "IN", "OUT" and "INOUT" macros. These are defined in ```bogus/fluff.h```, and implicitly included in ```feral/stdtypes.h```.
	- Return a FERALSTATUS type.
	- Be named accordingly, ("Ki" for internal kernel-related functions (stuff only drivers/kernel internals should talk with), "Ke" for general purpose functionality (typically syscalls).)

## What APIs will be supported for graphics?
I'm intending to either port Mesa/amdgpu/whatever or AMDVLK, or just writing a brand new Vulkan driver from scratch.
	OpenGL isn't intended to be supported (unless I do port the entirety of Mesa or something).
	The desktop will be composited in Vulkan (so a Vulkan *graphics* driver **is necessary**).
	Behind the scenes, we'll probably be making something more or less like DRI/DRM.
	
## General process for everything?
	The "native" API generally works as follows:

	```c
		UINT32 somethingLength;
		FERALSTATUS status = wpDoSomethingWithTwoValues(nullptr, somethingLength, someVal);
		if (status != STATUS_SUCCESS)
		{
			exit(-1);
		}
		/* Yes this is a VLA. Yes, I know it's not always great. */
		/* Stricter C code should use malloc()/free() instead. */
		Something someBuffer[somethingLength];
		status = wpDoSomethingWithTwoValues(someBuffer, somethingLength, someVal);
		if (status != STATUS_SUCCESS)
		{
			exit(-1);
		}
	```
	
	Generally speaking, the majority of internal kernel data structures
	will contain two values, a type sType describing (within a given context)
	what the object is, such as if it's a virtual memory table or filesystem
	table or something. The second type, sType, is a const void*. This is
	used to point to (currently) unknown data structures which may be added
	to the kernel in the future.
	
	Feral promises to keep compatibility between major versions, and have
	a stable internal and external ABI. However, this will only be
	achieved after version 1.0.0, when everything is put together
	in a meaningful way and well-tested such that it is fair
	to assume that many (if not all) the internal structures need not to be
	changed too often.

	This allows third party drivers loaded outside of the kernel to be
	fairly certain that for the near future, their drivers will probably
	be just fine. (if written for version 1.0.0, it'll probably work from
	1.0.0 to 1.999.999).
	
	When this promise must be broken, the kernel *will* go up a major version
	number. At that point, it is up to third party driver developers to update
	their drivers accordingly, if any update is necessary.
	
	
	


## Overall goals:
 - Lightweight core kernel. Majority of functionality in drivers. Processes communiate to libos (syscalls go to a kernel table which tells a libos driver that this was called.)
 
 - Much more clear, explicit kernel operations. Eliminate guessing--the kernel is *VERY* clear about exactly what it will do.

 - Convince user-mode apps that they're running on the real thing (when they're not.) Ie, run GNU Mach programs unmodified.

 - Stable, internal kernel ABI. Third party drivers should work across minor revisions. Use the pNext pointer for any serious changes.

 - Stable, versioning filesystem included, such that an accidental `deltree` doesn't delete everything forever. Assume the user makes no backups.

 - Support Vulkan out of the box. Use Vulkan as "the native graphics language" of the OS.
   (ie, everything graphics-wise, one way or another, goes through Vulkan rendering. GL is implemented on top of Vulkan, if at all.)
   Under Waypoint, you should be able to assume any graphics card plugged in using built-in drivers can support Vulkan 1.1.
   (we wouldn't expose it if it was a GPU that can't do Vulkan)

 - (Eventually) create fully Vulkan compliant drivers for the "Vega" and "Navi" family of GPUs.

 - Support x86-64 virtualization extensions ("Pacifica").

 - Highly modular architecture (portions of the kernel can be ripped out and reused elsewhere, ie, grafted into BSD or used in a commercial product or something.)
 
 - Integrate containers into our existing subsystem architecture, for example, to allow a user to execute a full, isolated not-quite-virtual machine to run untrusted applications in. (useful for security researchers using radare or whatever on malware.)

 - Something fun to work on that might be useful one day?

