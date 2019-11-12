# FERAL KERNEL

## What is Feral?
Feral is an experimental monolithic kernel, which is intended for use by my Waypoint operating system. It's primary goals
are flexibility, compatibility, security, and performance, roughly in that order.

Feral currently runs on x86-64 personal computers belonging to Family 17h ("Zen") under virtualization with KVM, 
but may work for other systems that are still PC-compatible (ie, not Liverpool or Scorpio APU). Feral is also 
known to work on an 6000-series x86-64 CPUs from a different CPU vendor, but this is not tested as often.
Specifically, the machine in question has a Braswell N3700 CPU from said different vendor.

Feral will probably also work on systems based upon future revisions of the Zen architecture, but this is not tested.
(Limited to Zen 1, Zen+ and Zen 2 are untested.)

Feral, for now, assumes your machine has something compatible with the two 8259A PICs present in many PC-compatibles.
On modern microcomputers, your southbridge probably has something compatible. In the future, Feral will probably
require the prescence of an APIC for SMP support. Feral also conflates "x86", "x86-64" and "PC compatible". For those wishing
to port Feral to a non PC-compatible x86 machine, you may encounter difficulties for now. In the future, Feral should have
some code restructured to no longer have this requirement, and potentially be ported to run on, for example, 
the Liverpool or Scorpio CPUs,  should some bootloader be present and capable of loading the kernel.

Feral is in the process of a rewrite to some core initialization functions, partly to allow porting to devices
which are truely headless, partly to allow for early setup of video output protocols, and to deprecate Multiboot 2 support.

In the future, Feral is intended to run on Aarch64 (BCM2711), POWER (Sforza uarch), and RV64GC (U540 SoC) hardware,
with ports written somewhere in that order.

Feral, when complete, may find use in gaming consoles, handheld devices, or as intended, personal computers.
Our goals are intended to be achieved by careful construction and design of the kernel, such that the kernel
naturally allows for maximum flexibility, and to ensure a clean, consistent, and coherent architecture to attract
developers to this new platform. Feral's philosophy regarding this can be summarized as 
"don't port your program, port the environment it runs on instead."

Feral is best described as being designed with a client-server model in mind for every task. Each component is
built in layers and groups, and to communicate between them naturally requires interacting with an object manager
to facilitate any interaction. A client typically sends a buffer to a server for the server to fill out, then
returns. This design allows portions of the kernel to be ripped out without fear of damaging the integrity of
the rest of the system, as well-written drivers will simply error out or find an alternative if a given module is
missing from the kernel. This may also allow easier construction of unikernels based on Feral, as unneeded components
can simply be removed. This potentially could have a use in single-tasking embedded systems in a single-user mode,
such that we can strip away much of the kernel while retaining satisfactory functionality.

Feral will introduce the idea of a command queue, in which certain system calls are queue-based, allowing for a page of memory to
be written with a number of commands, and the kernel executes them sequentially without being interrupted to return to usermode
without a reason to: this is intended to provide some form of 'inertia' between usermode and kernel-mode, such that a context
switch is avoided and thus improving performance. Those familiar with graphics programming are likely familiar with a similar concept.
The idea is that if we enqueue a variety of instructions, preferably on a common, machine-independent bytecode format, we avoid
more context switches than absolutely necessary and thus games can use more of the precious 16~17ms they have to render a frame
on more useful things, like loading assets, doing more draw commands, networking, AI, etc. This is similar to an GL call like
glBindBuffer(). The operating system's job is to *get out of the way of user mode apps (where security is left intact)*.

One of the primary advantages to this architecture is to collapse and simplify the given code sent to the kernel
in this manner into small, heavilly optimized routines using just-in-time compilation to minimize wasted time
doing unnecessary error checking or finding the appropriate context for a given operation.

Feral also does not support "legacy" hardware, such as IA-32. This is primarilly because this is really just a hobby project of mine,
on top of the fact that I don't really have any 32-bit PCs lying around. Also, IA-32 is over 30 years old without any new hardware releases
that I can actually buy for over 10. Since I don't have a particular reason to support it, for now we use "x86" and "x86-64" internally interchangably.
It is unlikely that Feral will ever run on 32-bit hardware of any kind (x86 or otherwise), as the security subsystem will incorporate features based upon
physical memory locations in order to thwart exploits making use of return-oriented programming and direct memory access, and lock certain areas of physical
memory to be non-writable and executable, such that an exploit breaking into the kernel is unlikely to work 100% of the time. In mind specifically is to change
which chunks of physical memory are allowed for things like shared objects, where they will be loaded, and what parts of physical memory will be used for
virtual executable or writable memory. When we are not using a shared object (ie, libc.so), a page cannot be writable and then later on executable: permission
*must* be granted by the security subsystem to do this, and done through a special, trusted shared object (usually the libos). At no point under
any circumstances should any memory be both writable and executable at the same time. In the future, we'll need to support boot-time relinking,
and especially incorporate address space layout randomization, to make it much more difficult to execute ROP chains in ring 0.

At it's heart, Feral is a hobby project with very large ambition. It's primary purpose for existing is to experiment with low level programming concepts,
resolve ("the hard way") some issues I have with other operating systems, and to explore the feasibility of certain ideas which are implemented. It's also
a nice project to demonstrate what I can do in systems programming in general, and to hopefully be a serious alternative to many desktop operating systems
in common use today, such that one day Feral can itself be developed on a machine running Feral, and then have kernel patches applied to itself without
any annoying reboots. It's also one of these areas where we *do* care about every little bit of performance that can be gained: we *do* care about the TLB,
we *do* care about the cache hierarchy, and we most certainly *do* care about how the compiler optimizes the code. Calling `strlen` on the same string
over and over is absolutely not acceptable, for what should hopefully be obvious reasons. There's also the "if we totally redesign the OS, can get get apps to run faster?" too.
Ideally, we'd get very good code quality in pursuit of all of these ideas and putting them together into a cohesive whole.

Most importantly: have fun, write good code, and get apps to run at least 5% faster than a competing commercial desktop operating system.

Feral is also partially the result of a dissatisfaction with the security model of many current operating systems, being insufficient to guarantee being secure out-of-the-box,
or outright preventing hardening of core infrastructure (ie, preventing the ability to disable any sort of data collection) and/or outright discouraging such practice. To me,
this seems totally unsatisfactory, wastes precious network bandwidth, and interrupts me when I do *not* want to be interrupted. What's particularly annoying is the removal of
an application, and either through some filesystem bug, an intentional design flaw, some kind of marketing scheme, or some combination of all 3, the application keeps returning.

Feral, in the future, will support SVM virtualization extensions ("Pacifica"), 

## Core architecture?
Architecturally, Feral is similar to a variety of prior systems originating between 1969 and 1993, with the most obvious being CMU's Mach project, and the Plan 9 project. Feral discards the "everything is a file"
design in favor of "everything is a 'network resource'", which is intended to align with more modern hardware and software designs. Everything can be sent packets and recieve packets:
writing to the filesystem is much the same as opening a web page and sending/recieving data to/from that. Feral also takes a lot of design inspiration from a variety of products which competed with
BSD and other then-current *NIX distributions, specifically intending to avoid "just creating another *NIX clone".

In other words, Feral flips the conventional "network stuff are just special files" on it's head: files are just special kinds of network stuff.

Feral is also intended to be very easy to develop for by game developers: in general, the APIs for Feral "feel like a graphics API". Once a port is opened, some data structures
and context info needs to be obtained from it, some settings set, and the system should do much of the work needed to get something going. However, Feral is very
explicit and verbose in how operations are done: care should be taken that undefined behavior is avoided, since the behavior may change in between minor and especially major releases.
This architecture allows for a game developer to take and bundle a specific libos and runtime environment for their programs, which would allow them to drastically
simplify application code, as they become in control of *exactly* how the kernel processes their data: there is no ambiguity between file access priorities, networking requirements,
and so on, and a simpler application API can be exposed to their games which help simplify code. Every application on Feral potentially can be it's own "ecosystem", and share data
with each other through simple interfaces that avoid getting in the way of usermode applications.

In the future, we would also like to address theming issues at the operating system level. There's no real reason why the kernel shouldn't be aware of the shell running on top of it,
so why don't we let the kernel create a user theme context and allow applications to pick suggested colors from this? This way, we can allow enlightened Feral/Waypoint programs to use
a system call to query if the user wants a light mode or dark mode, and we can finally stop having black text on black backgrounds because of theming issues.

Feral's layout and design is also intended to be simple for new developers to read the code and understand it, or perhaps even use it for their own projects.
Care is being taken to avoid creating a mess in the source code, and that all important details are explained and documented well.
Currently, there is a lot of work to be done to achieve this, in order to comply with my own programming style standards.

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
(for now, we have the bogus variable ```ILOVEBEAR18``` which should be checked to 'false' before doing stuff, just in case for whatever reason
 there's something somewhere that requires old functionality and hasn't been fixed yet. This should eventually be removed and then hide experimental features behind an ifdef.)

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

