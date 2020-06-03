# FERAL KERNEL

## What is Feral?
Feral is an experimental monolithic kernel, which is intended for use by my Waypoint operating system. It's primary goals
are flexibility, compatibility, security, and performance, roughly in that order.

Feral is designed to run on recent PCs, as of 2020, with the core architecture belonging to either the "Skylake" or
direct descendants such as "Kaby Lake", "Ice Lake", and "Coffee Lake", as well as the "Zen" microarchitecture, and it's
immediate descendants, such as "Zen+", "Zen 2", and "Zen 3", where the firmware is either compatible with BIOS, as a
traditional PC, or is compatible with UEFI version 2.8. As such, certain x86 systems like "Liverpool" or "Scorpio" are
not supported, as well as legacy x86 PCs like the PC AT and it's many clones.

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
most functionality into drivers, rather than directly baked into the kernel itself, 
to allow for more flexibility, and to expose resources as a network location and socket, to
better reflect how modern processes and storage work.

For example, to open a file, one queries the file server, which is a process
owned by the kernel, which opens a socket which accepts incoming traffic.
It can then be read from, or written to, depending on how the socket was
opened.

## Why Feral?

Feral is created for me to learn more about operating systems design, while departing
from the traditional "make another *NIX clone" way of doing this: there's nothing 
wrong with that, but I'd like to explore different ways of exposing hardware and
system features to programs besides just making a file somewhere. In addition,
I'd like to explore just how different a new OS can be from traditional systems
while being easy to port existing software to.

While this image is outdated, since some of these subsystems should get absorbed
into the kernel, for the most part this is accurate to the intention:
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
1 of the Zen microarchitecture. It is probable that Feral works just fine on other
x86 implementations, but this is not tested.

## Is there any naming convention?
Folders should be named in a way to avoid unnecessary characters (ie, 'inc' vs 'include'.)
Likewise, for a given module, the folder name should reflect the prefix, such that
memory management is in "mm", and object management is in "ob", and so on.

Functions thaat should be exported between modules should generally have the
naming convention of (SUBSYSTEM)(FUNCTION-PURPOSE)(OPTIONAL-ARGUMENT-TYPE),
such as "KeStartProcessor". When the function is architecture-specific, then
it should be prefixed with the architecture it belongs to, such as 
"aarch64StartPaging()", or "x86SetupIDT()". For more details, see Formatting.txt

In addition, functions should use the macros IN, INOUT, OPT, etc. to clearly
identify what a function does with an argument. These macros expand to nothing,
and are only there to help in documentation.

## Overall goals:
 - Lightweight core kernel. Majority of functionality in drivers. Processes communiate to libos (syscalls go to a kernel table which tells a libos driver that this was called.)
 
 - Much more clear, explicit kernel operations. Eliminate guessing--the kernel is *VERY* clear about exactly what it will do.

 - Drivers handle system calls to allow running "foreign" programs unmodified, such as the GNU Mach version of GCC, or everyone's favorite DOS game set on Mars.

 - Stable, internal kernel ABI. Third party drivers should work across minor revisions. Use the pNext pointer for any serious changes.

 - Stable, versioning filesystem included, such that an accidental `deltree` doesn't delete everything forever. Assume the user makes no backups.

 - (Eventually) create fully Vulkan compliant drivers for the "Vega" and "Navi" family of GPUs.

 - Support x86-64 virtualization extensions ("Pacifica").
 
 - Something like a chroot jail or containers, to allow Feral to be useful for server operations.

 - Something fun to work on that might be useful one day?