# FERAL KERNEL

## What is Feral?
Feral is an experimental monolithic kernel aimed at being used for x86-64-based computers that are intended to be used as a gaming system.
It's primary purpose is having a strong graphics stack built specifically for maximizing performance out of GCN5+ GPUs, minimizing OS overhead of video games, allowing for careful control of the underlying hardware, and incorporate other features which might be useful for video games.

Architecturally, Feral is inspired by a variety of prior systems, most of which are not *nixoid in nature (and almost all of them originating from between 1969 and 1993). 
This is mostly "to be different" and to try out ideas than to have any real functional purpose behind it. 

It's really just a hobby project intended to play around with systems design and porting programs to it (in particular, open source games), but is designed in such a way that it wouldn't require (in theory) a complete rewrite to be useful for some actual serious product or solution or something. I just want to build something that can outperform big, much more serious platforms. Even if the difference is insignifigant. (Performance and security is
always priority #1.)

One of the things in particular that I thought it would be fun to try is to completely divorce the kernel from any particular executable format: the kernel simply allows subsystems to be attached into it
in order to handle a particular format (ELF, MACHO, PE, COFF, etc.), and these can further be hooked into kernel-based virtual machines to emulate other CPU architectures (POWER, Aarch64, legacy x86, etc.). The new problem
becomes how these drivers can be dynamically created and placed into the OS, and this will probably be with some special object file just for Feral.
This flips the problem of porting programs to Feral on it's head: rather than "what can we do to get people to port their games", it's "how do we build a system that convinces the anti-cheat stack that we're the real thing?"
and thus solves the chicken-and-egg problem of a new platform having no programs for it by avoiding the problem altogether. (no one wants to make games for a platform for no users, and no users want to go on a platform with no games. It's an annoying catch-22 and this is probably the only "easy" way to solve that without throwing tons of money at the problem. If you say "you literally don't have to do anything", everyone's happy.) 
It's, after all, vastly preferable to have broken games and no online functionality than no games at all.
These "service providers" would register themselves with the kernel by using a function like ```SrvcRegisterProvider()``` or some other name like that. None of this exists yet.
This way, there is no urgent need to recompile and directly port the toolchain used to build Feral. We just get some Linux compatibility layer and proceed as normal.

In an ideal world, we'd figure out a way to assimilate a real, foreign OS and manage it ourselves or something to *guarantee* compatibility without virtualization.
This is likely impossible (it would need to be designed to be completely agnostic to all file formats, capable of understanding the kernel and integrating binary blobs extracted from the file...), so
this is the next best thing. (avoid virtualization because performance loss (even if it's only like 4%), you can't share the gpu, etc. etc.)

I'm mostly doing this to get *much* better at C and assembly code, and especially interoperation between C and assembly, while also doing a little bit of Rust for memory-sensitive functionality.

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

	- Minimal characters prefixed to define what it does ('Hal', 'Vd', 'Ke', 'Vga', etc.)

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



## Overall goals:
 - Lightweight kernel, insist that processes do most of the work. (service provider does most of the kernel-mode work: programs shouldn't talk to kernel directly that much.)

 - Enforce a lot of verbosity out of user-mode programs (if what they could want could be ambiguous, there should be a function for every possible meaning.)
   (We also (when we get to the shell), don't do something silly like require two bit flags for vertical scroll bars. This has no functional purpose.)
   (We have no idea what shell replacements might want to do with native Waypoint programs: just feed them as much information as possible.)

 - Monolithic kernel design, supporting modules, but prefering to avoid them.
   (This is to avoid driver conflicts... simply refer to a kernel version and maybe provide a loadable module just in case. All important drivers should be in upstream.)

 - Support a new, high performance filesystem intended for modern storage hardware. (ie, SSDs, shingled hard drives, etc.) (everyone copies one with a 'Z' in the BSD source code, so why don't we too?)
   (we'd probably use a different hashing algorithm (Keccak/SHA3 probably) and try very hard to support a wider range of features, more stable pooling, etc. etc.)

 - Support Vulkan out of the box. Use Vulkan as "the native graphics language" of the OS.
   (ie, everything graphics-wise, one way or another, goes through Vulkan rendering. OpenGL is implemented on top of Vulkan, if at all.)
   Under Waypoint, you should be able to assume any graphics card plugged in using built-in drivers can support Vulkan 1.1.
   (we wouldn't expose it if it was a bad GPU that can't do Vulkan)

 - (Eventually) create fully Vulkan compliant drivers for the "Alpha Lyrae" family of GPUs. (or just port radv)

 - Integrate a high-performance virtual machine into the kernel, intended for use for "write once, run anywere" kinds of games that can take the performance hit. (Will never be close to native, but we can try...)
 
 - Maximize performance for most compute-intensive 3D applications, specifically do everything we can to make games faster from the OS' side. (this won't be much, but it should still be a priority)

 - Be "platform agnostic" in that the kernel can be combined with an arbitrary program loader and be able to execute "foreign applications", ie, Linux ELF programs or NTOS PE programs.
   (we're more awesome if we can even run programs for a totally different CPU architecture, ie, POWER9. A very important historic operating system was very much capable of this (at least I think).)

 - Highly modular architecture (portions of the kernel can be ripped out and reused elsewhere, ie, grafted into BSD or used in a commercial product or something.)

 - Something fun to work on that might be useful one day?

