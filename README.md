# FERAL KERNEL

## What is Feral?
Feral is an experimental monolithic kernel, which is intended for use by my Waypoint operating system. It's primary goals
are flexibility, compatibility, performance, and security.

Feral currently runs on x86-64 personal computers belonging to Family 17h ("Zen") under virtualization with KVM, 
but may work for other systems that are still PC-compatible (ie, not Liverpool or Scorpio APU). Feral is also 
known to work on an 6000-series x86-64 CPUs from a different CPU vendor, but this is not tested as often.

Feral will probably also work on systems based upon future revisions of the Zen architecture, but this is not tested.
(Limited to Zen 1, Zen+ and Zen 2 are untested.)

Feral for now, if on a PC, assumes your hardware has an APIC, storage devices are usually ATA or SCSI/SAS, and is booted
from Multiboot 2. An experimental bootloader to boot directly from EFI firmware is in progress, but is at a low priority.

In the future, Feral is intended to run on MIPS, Aarch64, POWER9, and RV64GC hardware.

Feral, when complete, may find use in gaming consoles, handheld devices, or as intended, personal computers.
Our goals are intended to be achieved by careful construction and design of the kernel, such that the kernel
naturally allows for various things to help usermode applications improve performance.

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

At it's heart, Feral is a hobby project. It mostly exists to give me a prime, "real world" example for applying low-level programming concepts,
getting rid of all the bloat of modern operating systems (various virtual machines shouting "write once, run anywhere!" come to mind), and to
create a real, working project as a demonstration of what I can do. It's also for one day being able to use software that I personally wrote on a daily
basis (dogfooding), and to have a nice little playground where I can focus on all the little details of the hardware without worrying about alienating
people because it's "obscure code". For the OS, we *do need to care* about the TLB and cache efficiency and all sorts of low level details, and calling strlen()
over and over again is not acceptable. Most importantly: have fun, write good code, and get apps to run at least 5% faster than a competing commercial desktop operating system.
There's also the "if we totally redesign the OS, can get get apps to run faster?" too.

Feral is also partially the result of a dissatisfaction with the security model of many current operating systems, being insufficient to guarantee being secure out-of-the-box,
or outright preventing hardening of core infrastructure (ie, preventing the ability to disable any sort of data collection) and/or outright discouraging such practice. To me,
this seems totally unsatisfactory, wastes precious network bandwidth, and interrupts me when I do *not* want to be interrupted.

Feral, in the future, will support SVM virtualization extensions ("Pacifica"), 

## Design influences?
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

Feral is intended to drastically depart from the traditional *NIX architecture in favor or being far more verbose, explicit, and direct to usermode apps. In my opinion,
something like an "ioctl" call is rather ugly, and trying *too hard* to make things into files ironically ends up being more difficult than just saying "it supports reads and/or writes, and
is located here through this protocol." If we have something like *ioctl*, then **everything** should be like *ioctl*, or **nothing** should be. Feral prefers something like the former.
While this approach might be insecure if badly handled, if we design this well, we don't handle it badly.

We could potentially even provide susbsystems in the kernel or core usermode libraries and bundle a CPU emulator, which would allow us to execute code intended for
a totally different CPU architecture. This could allow Feral to be used for backwards compatibility with legacy systems, and keep old programs running on
new hardware and new software, aiding in software preservation. While we can't get 100% compatibility, we can do "good enough" to hopefully be a good
starting ground for emulator developers to aid in cross-compiled software development, and be especially of use for game developers who have since lost the original 
source code for some of their older games.

Feral is best described as a "modular macrokernel", or just a "monolithic kernel with loadable drivers at runtime", which is a little bit harder to say.

![Feral Architecture](Documentation/images/feralarch.png)

## Building?
Run ./build.sh $ARCH $COMMAND,
where $ARCH is the arch (x86_64), and $COMMAND
is the command to run (qemu, qemu-nokvm, clean, etc.)

This is temporary, and will be replaced with a proper
build system (Kconfig) later.

## Is there any naming convention?
Folders should be named in a way to avoid unnecessary characters (ie, 'inc' vs 'include'.) This is just because I tend to have too many tabs of ~~Nautilus~~ 
flavor-of-the-month-file-explorer open and each one very small. Not important. 3 monitors isn't enough to make that OK.

Functions should *always* be prefixed with 'internal' if they are temporary. (ie, to test something.) These should be wrapped in an
```
#ifdef DEBUG

VOID internalDoSomething()
{
	return;
}


internalDoSomething();
#endif DEBUG
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

## How will frame creation be done on a desktop (when it gets there)
When this eventually happens, I'd like to have GUIs created something like as follows:

```c
// (this is pseudocode of course). 'Wp' is for 'Waypoint'. This probably won't compile with a real compiler.
#define APPLICATION_NAME GenericSample

HFRAME Frame = UiCreateFrame(L"Hello, world!", 600, 800, NULL, NULL);		// Create a top-level frame on the desktop.
struct SystemProperties props;							// System properties (themes, frame control location, etc.)
WpStatus status = WpGetSystemProperties(APPLICATION_NAME, &props);		// Put the properties into the struct.

struct ApplicationProperties* appProps;

appProps = (struct ApplicationProperties*)malloc(sizeof(struct ApplicationProperties));

appProps->SystemProps = props;
appProps->AppPurpose = FERAL_GENERIC_GUI_APPLICATION;
appProps->AppVersion = FERAL_MAKE_VERSION(1, 0);
// etc. etc.

if (status < 0)	//This will signal that there is an error. Positive values are just warnings.
{
	fprintf(stderr, "Error getting system properties. Is there a GUI?");
	exit(-1);								// Quit if we're not running a desktop of some sort.
}

UINTN systemPeripherals = 0;
status = WpQuerySystemPeripherals(APPLICATION_NAME, appProps, &systemPeripherals, NULL);	// Query what devices this application is allowed to use. The API handles actual permissions itself.

if (status < 0)
{
	fprintf(stderr, "Unable to query system devices.");
	exit(-1);
}

struct WpSystemPeripherals[systemPeripherals] peripherals;
status = WpQuerySystemPeripherals(APPLICATION_NAME, appProps, &systemPeripherals, peripherals);
if (status < 0)
{
	fprintf(stderr, "Unable to enumerate system devices.");
	exit(-1);
}

// By default, all peripherals are ignored. You need to allow the keyboard to talk to the app (beyond system controls like Alt+F4, mouse to click 'Exit', etc.) by calling the appropriate functions.
// This is done basically just to make it as verbose as possible with attaching events to things. All user interactions should be expected and handled by the developer.
// (also to make it more "Vulkan-like"--we want applications to be *very* verbose as a way to make it very clear to any theme engine or shell replacement what an app is trying to do.)

// Bottom left corner is (0, 0).
// Draw a black rectangle at (90, 90) of size (100, 200).
UiDrawReactangle(&Frame, UI_COLOR_BLACK, 90, 90, 100, 200);

// Come up with some idea to handle widgets (either borrow ideas from GTK or Swing).
// Maybe try some sort of 'subframe' thing, and have the desktop daemon define a whole bunch to use (up to GUI developers).
// This also happens to make it very easy to embed a 3D environment into an application (no ugly toolkit hacks), so that might be good for game engine developers.

HCOLORREF* FrameColors = UiGetRawImage(&Frame);	//This is just an array of colors which can then be thrown numbers at to have them show up. Use this in a Vulkan app. Expect this to be renamed.

//blah blah blah, wait(), all the fun stuff. Multithreading, all objects need to be thread safe, etc. etc.

//Time to free the app. Destroy in reverse order of allocation.
WpDestroySystemPeripherals(peripherals);
WpDestroyApplicationProperties(appProps);
WpDestroySystemProperties(props);
UiDestroyFrame(Frame);
return 0;

```

The usual process of doing things in Waypoint (and Feral in general) is:
	
		- Open port
		- Create context
		- Do things
		- Close context
		- Release port

For example, opening a file would be as simple as opening a resource on the filesystem server (sometimes a kernel subsystem, often the local libos, possibly FUSE),
constructing a context (setting up case sensitivity, naming restrictions, error handling, etc.), and writing to the port.
This is more flexible than the "everything is a file" approach, as it allows for a subsystem provider to clearly define what "write" and "read" mean:
'reading' data from an arbitrary location could mean many different things, and without corrections, could result in various oddities (reading int
as little endian in big endian format, for example). Subsystems and servers take care of that.
It also allows us to deal with badly written programs that assume spaces don't appear in a given path: B:/Main Storage/Files/Stuff/Something Cool/ is a valid
file path, and should *not* be interpretted as B:/Main, Storage/Files/Stuff/Something, and Cool/.
(This will introduce new problems, like when we actually *do* want to interpret as 3 new paths, but we can figure that out along the way.)


## Overall goals:
 - Lightweight kernel, insist that processes do most of the work. (service provider does most of the kernel-mode work: programs shouldn't talk to kernel directly that much.)
 
 - Eliminate legacy "bloat" from traditional architecture and design: Our goal is to be **lightweight** and **mostly compatible** and **secure**.

 - Enforce a lot of verbosity out of user-mode programs (if what they could want could be ambiguous, there should be a function for every possible meaning.)
   (We have no idea what shell replacements might want to do with native Waypoint programs: just feed them as much information as possible.)

 - Monolithic kernel design, supporting modules, but prefering to avoid them.
   (This is to avoid driver conflicts... simply refer to a kernel version and maybe provide a loadable module just in case. All important drivers should be in upstream.)

 - Support a new, high performance filesystem intended for modern storage hardware. (ie, SSDs, shingled hard drives, etc.)  (btrfs???)

 - Support Vulkan out of the box. Use Vulkan as "the native graphics language" of the OS.
   (ie, everything graphics-wise, one way or another, goes through Vulkan rendering. OpenGL is implemented on top of Vulkan, if at all.)
   Under Waypoint, you should be able to assume any graphics card plugged in using built-in drivers can support Vulkan 1.1.
   (we wouldn't expose it if it was a GPU that can't do Vulkan)

 - (Eventually) create fully Vulkan compliant drivers for the "Vega" and "Polaris" family of GPUs. (or just port radv)

 - Integrate a high-performance virtual machine into the kernel, intended for use for "write once, run anywere" kinds of games that can take the performance hit. (Will never be close to native, but we can try...)
 
 - Maximize performance for most compute-intensive 3D applications, specifically do everything we can to make games faster from the OS' side. (this won't be much, but it should still be a priority)

 - Be "platform agnostic" in that the kernel can be combined with an arbitrary program loader and be able to execute "foreign applications", ie, Linux ELF programs or NTOS PE programs.
   (we're more awesome if we can even run programs for a totally different CPU architecture, ie, POWER9. A very important historic operating system was very much capable of this (at least I think).)

 - Highly modular architecture (portions of the kernel can be ripped out and reused elsewhere, ie, grafted into BSD or used in a commercial product or something.)
 
 - Integrate containers into our existing subsystem architecture, for example, to allow a user to execute a full, isolated not-quite-virtual machine to run untrusted applications in. (useful for security researchers using radare or whatever on malware.)

 - Something fun to work on that might be useful one day?

