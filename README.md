# FERAL KERNEL

## What is Feral?
Feral is an experimental monolithic kernel intended to be used for my Waypoint operating system. It's primary goals are flexibility,
reliablility, and good security, and above all else, performance. Feral is intended to run for many architectures, but is currently being
developed primarilly for the x86_64 platform, and more specifically for Family 17h x86-64 PCs ("Zen" architecture)

In the future, Feral is intended to run on MIPS, Aarch64, POWER9, and RV64GC hardware.

Feral, when complete, may find use in gaming consoles, handheld devices, or as intended, personal computers.
Our goals are intended to be achieved by careful construction and design of the kernel, such that the kernel
naturally allows for various things to help usermode applications improve performance.

In particular, Feral is built with a client-server model for most tasks, and is built in layers and components.
This is to facilitate packaging and organization of the kernel such that it would be simple to create a unikernel
based upon Feral, and only link in the absolute necessary parts as well as one static user mode (or several) applications
to achieve a meaningful task.

Feral introduces the idea of a command queue, in which certain system calls are queue-based, allowing for a page of memory to
be written with a number of commands, and the kernel executes them sequentially without being interrupted to return to usermode
without a reason to: this is intended to provide some form of 'inertia' between usermode and kernel-mode, such that a context
switch is avoided and thus improving performance. Those familiar with graphics programming are likely familiar with a similar concept.
The idea is that if we enqueue a variety of instructions, preferably on a common, machine-independent bytecode format, we avoid
more context switches than absolutely necessary and thus games can use more of the precious 16~17ms they have to render a frame
on more useful things, like loading assets, doing more draw commands, networking, AI, etc. This is similar to an GL call like
glBindBuffer(). The operating system's job is to *get out of the way of user mode apps (where security is left intact)*.

Feral also does not support "legacy" hardware, such as IA-32. This is intended to keep development focused on current hardware and to
avoid focusing on legacy structures and ideas which would hold back design of the kernel. What "legacy" actually means depends on
how such a processor is currently being used: IA-32 CPUs for desktop gaming use are no longer in production, and thus not supported.

At it's heart, Feral is a hobby project, intended to be useful and fun for learning how to write good C code, building a large, complex project,
overall design, systems, and hardware-level programming. In addition to this, it's also for fun for me to see how much faster I can get certain applications
to run simply by changing platforms and doing minimal effort to port programs over. In particular, I would like to see a 5% performance improvement
or better for certain open sourced games from the 90s as compared to a particularly prolific desktop operating system in active development from a 
well-known company in Washington state. This is a good and interesting challenge, and aiming at a moving target is more fun. Feral is also
being developed because bottom-up integration of every system from the operating system and up allows for good optmization, a good foundation in
designing new applications on top of it, and a well-known, solid base upon which new things can be built from: this is in contrast to re-using an existing
project in where some design choices may not align with the intended use case. Building *everything* from the ground up allows us to tailor every
piece of the system to our exact needs, and thus trim away unnecessary bloat from design and implementation. For example, even needing a multi-user
environment for Feral could be debated, as a multi-user environment has no direct benefit for a __personal__ computer, and costs much in performance:
consider the performance of an application on Haiku versus a *NIX version on identical systems, specifically in a case where checking user IDs
is a common operation (ie, a word processor, where writing and reading files is a fairly common operation). If there is only one user for the machine,
and we always trust the user knows what they're doing (until they mess with system files), do we really need a multi-user environment?

## Design influences?
Architecturally, Feral is similar to a variety of prior systems originating between 1969 and 1993, with the most obvious being CMU's Mach project, and the Plan 9 project. Feral discards the "everything is a file"
design in favor of "everything is a 'network resource'", which is intended to align with more modern hardware and software designs. Everything can be sent packets and recieve packets:
writing to the filesystem is much the same as opening a web page and sending/recieving data to/from that. Feral also takes a lot of design inspiration from a variety of products which competed with
BSD and other then-current *NIX distributions, specifically intending to avoid "just creating another *NIX clone".

Feral is also intended to be very easy to develop for by game developers: in general, the APIs for Feral "feel like a graphics API". Once a port is opened, some data structures
and context info needs to be obtained from it, some settings set, and the system should do much of the work needed to get something going. However, Feral is very
explicit and verbose in how operations are done: care should be taken that undefined behavior is avoided, since the behavior may change in between minor and especially major releases.

Feral's layout and design is also intended to be simple for new developers to read the code and understand it, or perhaps even use it for their own projects.
Care is being taken to avoid creating a mess in the source code, and that all important details are explained and documented well.

## Why Feral?

Feral, while aiming to be a monolithic kernel, has much in common with the nanokernel and microkernel ideas. Feral itself only supports a specific format to load drivers in,
and these drivers define executable formats, file formats, network structures, and more, and simply hook into the existing Feral kernel. While most services are made available in
the kernel, the kernel is very modular and by itself does not provide more than the absolute minimum for a given service. This allows Feral to even, in theory, support
CPU dynamic recompilation and run code intended for an entirely different platform, or run executables not directly intended for Feral, eliminating the need to port
applications to Feral entirely. However, I only intend to distribute Feral with enough to build Feral on itself, as well as provide a Waypoint reference libos.

Feral is best described as a "modular macrokernel", or just a "monolithic kernel with loadable drivers at runtime".

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

## How will frame creation be done on a desktop (when it gets there)
When this eventually happens, I'd like to have GUIs created something like as follows:

```
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
 
 - Eliminate legacy "bloat" from traditional architecture and design: Our goal is to be **lightweight** and **mostly compatible**. If it gets in the way of lightweight, we won't implement it.

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

