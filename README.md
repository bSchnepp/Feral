# FERAL KERNEL

## What is Feral?
Feral is an experimental monolithic kernel aimed at being used for x86-64-based computers that are intended to be used as a gaming system.
It's primary purpose is having a strong graphics stack built specifically for maximizing performance out of GCN5+ GPUs, minimizing OS overhead of video games, allowing for careful control of the underlying hardware, and incorporate other features which might be useful for video games.

## Is there any naming convention?
Folders should be named in a way to avoid unnecessary characters (ie, 'inc' vs 'include'.) This is just because I tend to have too many tabs of Nautilus open and each one very small. Not important.

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

Functions intended to be stable should use:

	- Minimal characters prefixed to define what it does ('Hal', 'Vd', 'Ke', 'Vga', etc.

Kernel-related functions should ALWAYS
	
	- Insist on using arguments as inputs/outputs, using the "IN", "OUT" and "INOUT" macros.
	- Return a FERALSTATUS type.
	- Be named accordingly, ("Ki" for internal kernel-related functions (stuff only drivers/kernel internals should talk with), "Ke" for general purpose functionality (typically syscalls).)

## What APIs will be supported for graphics?
I'm intending to either port Mesa/amdgpu/whatever or AMDVLK, or just writing a brand new Vulkan driver from scratch.
	OpenGL isn't intended to be supported (unless I do port the entirety of Mesa or something).
	The desktop will be composited in Vulkan (so a Vulkan driver **is necessary**).

## How will frame creation be done on a desktop (when it gets there)
When this eventually happens, I'd like to have GUIs created something like as follows:

```
// (this is pseudocode of course). 'Wp' is for 'Waypoint'.
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
// (also to make it more "Vulkan-like")

// Bottom left corner is (0, 0).
// Draw a black rectangle at (90, 90) of size (100, 200).
UiDrawReactangle(&Frame, UI_COLOR_BLACK, 90, 90, 100, 200);

// Come up with some idea to handle widgets (either borrow ideas from GTK or Swing).
// Maybe try some sort of 'subframe' thing, and have the desktop daemon define a whole bunch to use (up to GUI developers).
// This also happens to make it very easy to embed a 3D environment into an application (no ugly toolkit hacks), so that might be good for game engine developers.

HCOLORREF* FrameColors = UiGetRawImage(&Frame);	//This is just an array of colors which can then be thrown numbers at to have them show up. Use this in a Vulkan app. Expect this to be renamed.

//blah blah blah, wait(), all the fun stuff.

//Time to free the app. Destroy in reverse order of allocation.
WpDestroySystemPeripherals(peripherals);
WpDestroyApplicationProperties(appProps);
WpDestroySystemProperties(props);
UiDestroyFrame(Frame);
return 0;

```



## Overall goals:
 - Lightweight kernel, insist that processes do most of the work.
   (NOT in the same way as a microkernel, more of the kernel avoiding management where unnecessary)

 - Monolithic kernel design, supporting modules, but prefering to avoid them.
   (This is to avoid driver conflicts... simply refer to a kernel version and maybe provide a loadable module just in case.)

 - Support a new, high performance filesystem intended for modern storage hardware.
 
 - Support Vulkan out of the box. Use Vulkan as "the native graphics language" of the OS.
   (ie, everything graphics-wise, one way or another, goes through Vulkan rendering. OpenGL is implemented on top of Vulkan, if at all.)

 - Integrate a high-performance virtual machine into the kernel, intended for use for "write once, run anywere" kinds of games that can take the performance hit. (Will never be close to native, but we can try...)
 
 - __MAXIMIZE PERFORMANCE!!!!!__
