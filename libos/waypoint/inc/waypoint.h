/*
Copyright (c) 2018, Brian Schnepp

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

#ifndef _WAYPOINT_H_
#define _WAYPOINT_H_

#include <feral/stdtypes.h>
#include <bogus/fluff.h>
/*#include <conio.h>*/

#include <stdio.h>

#define FERAL_MAKE_VERSION(Major, Minor, Patch) (((Major) << 22) | ((Minor) << 12) | (Patch))


/* last semicolon is intentionally missing (force style convention in that every
   statement does end in a semicolon.) */
   
   /* pNext is for extensions for the Waypoint API. The intent is that programming
      for a Waypoint system "feels" like writing a glNext or gl app,
      
      (largely for familiarity)
   */
#define WP_BASE_STRUCT_MEMBERS		\
	WpDataType	sType;		\
	VOID*		pNext

/* Applications can 'extend' these structures however they'd like. 
   As long as the sType is correct, it should be OK.
   
   A compliant Waypoint OS should retain previous API-breaking
   libraries such that an application can specify which environment
   it wants in case we change the __ABI__ in the future.
   
   ie, if SystemProperties is modified, then the OS should link with
   a previous version of the library refactored to support the new
   stuff.
   
   
   (we may change this to be opaque structs, and ask the programmer to
    just build a buffer and supply a used size)
 */
typedef enum
{
	WP_DATA_TYPE_APPLICATION;
	WP_DATA_TYPE_SYSTEM_PROPERTIES;
	WP_DATA_TYPE_APPLICATION_PROPERTIES;
	WP_DATA_TYPE_CONTENT_LANGUAGE;
	WP_DATA_TYPE_SYSTEM_STATE;
	WP_DATA_TYPE_GLOBAL_APPLICATION_STATE;
	WP_DATA_TYPE_APPLICATION_STATE;
	
	
	WP_DATA_TYPE_WAYPOINT_FRAME;
}WpDataType;


typedef enum
{
	// 'generic' is there just to describe the program running.
	FERAL_GENERIC_TERMINAL_APPLICATION,
	FERAL_GENERIC_GUI_APPLICATION,
	FERAL_GENERIC_GAME_APPLICATION,
	FERAL_GENERIC_MEDIA_APPLICATION,
	FERAL_GENERIC_WEB_BROWSER_APPLICATION,
	FERAL_GENERIC_HIGH_PERFORMANCE_CREATIVE_APPLICATION,	// Stuff like the GNU GIMP and whatnot.
	FERAL_GENERIC_APPLICATION,

	FERAL_FOREIGN_APPLICATION,	// this is a program that's lazilly ported to Waypoint and pretends to be a native waypoint app, but is really running through basically libWINE.

	// These have special meanings.
	FERAL_EMULATED_GAME_APPLICATION,	// This game is running on a emulated processor (ie, a JIT-ed POWER or Aarch32 or something).
	FERAL_NONCRITICAL_GAME_APPLICATION,	// This game is not heavy on resources, and thus is OK to de-prioritize (like a simple card game).
}ApplicationType;

// TODO: do this better (this should be part of libc / c runtime)
// (in order: argc, argv, Parent task, owning user, and current directory)
UINT32 WayMain(IN UINT32 ArgumentCount, IN WSTRING* Arguments, INOPT HTASK Parent, INOPT HUSER User, INOPT HANDLE Directory);

typedef UINT64 WPSTATUS;

// Well, no way to avoid trademarks here...
typedef enum GPUVENDOR
{
	GPU_VENDOR_NVIDIA,	// Kepler and earlier only for now. Pretend pre-bankruptcy 3DFX is NVidia if someone creates a driver.
	GPU_VENDOR_AMD,		// Polaris (& Polaris +) and later only. Pretend pre-acquisition ATI is still AMD if someone makes a driver.
	GPU_VENDOR_INTEL,	// Jupiter Sound, Artic Sound, HD Graphics, etc.
	GPU_VENDOR_ARM,		// Mali
	GPU_VENDOR_VIA,
	GPU_VENDOR_MATROX,
	GPU_VENDOR_QUALCOMM,	// Adreno
	GPU_VENDOR_POWERVR,
	GPU_VENDOR_UNKNOWN,	// custom chips (graphics ASICs and all for consoles, you're on your own for any generic driver)
	GPU_VENDOR_NONE,	// There is no GPU. Vulkan "graphics" are executed on the CPU. We may be compute only (headless).
	
	/* Reserved block (for vendors that do not currently make GPUs or do not exist yet) */
	GPU_VENDOR_RESERVED1,
	GPU_VENDOR_RESERVED2,
	GPU_VENDOR_RESERVED3,
	GPU_VENDOR_RESERVED4,
	GPU_VENDOR_RESERVED5,
	GPU_VENDOR_RESERVED6,
	GPU_VENDOR_RESERVED7,
	GPU_VENDOR_RESERVED8,
	GPU_VENDOR_RESERVED9,
	GPU_VENDOR_RESERVED10,
	GPU_VENDOR_RESERVED11,
	GPU_VENDOR_RESERVED12,
	GPU_VENDOR_RESERVED13,
	GPU_VENDOR_RESERVED14,
	GPU_VENDOR_RESERVED15,
	GPU_VENDOR_RESERVED16,
	GPU_VENDOR_RESERVED17,
	GPU_VENDOR_RESERVED18,
	GPU_VENDOR_RESERVED19,
	GPU_VENDOR_RESERVED20,
	
	GPU_VENDOR_VIRTUAL,	// Our GPU is actually a CPU emulating a GPU.
	GPU_VENDOR_MANYCORE,	// We're (ab)using a CPU somewhere else on the network (or this PC) to handle graphics. Ie, something like Larrabee, or a TR4 CPU for it.
};

typedef struct SystemProperties
{
	WP_BASE_STRUCT_MEMBERS;
	
	UINT64 NumProcessors;
	UINT64 NumThreads;
	
	UINT64 NumGraphicsProcessors;
	GPUVENDOR* Vendors;

	STRING* GraphicsProcessorNames;
	UINT64* GraphicsProcessorMemorySizes;
	UINT64* AvailableGraphicsProcessorMemorySizes;

	UINT64 MaxMemory;		/* This includes swap space */
	UINT64 MaxNoSwapMemory;		/* Max memory on the system */
	UINT64 MaxAvailableMemory;	/* Application may only be allowed to use a subset of the available memory. (limits) */

}SystemProperties;

typedef struct ApplicationProperties
{
	WP_BASE_STRUCT_MEMBERS;
	
	SystemProperties SystemProps;
	ApplicationType AppPurpose;
	UINT32 AppVersion;
	UINT64 RESERVED;	// No, this isn't reserved for _you_, just that we save space so the ABI doesn't change.
}ApplicationProperties;

typedef struct ContentLanguage
{
	WP_BASE_STRUCT_MEMBERS;
	
	/* The abbreviation of the language, (ie, en_US, en_UK, etc.) */
	STRING LangShort;

	/* The full name of the language. (ie, "English - US"). This appears in that language's words 
	(ie, you would see "Espanol - MX", not "Spanish" with the proper unicode symbols (the correct 'n-yuh' character) and all.*/
	/* Note that we ignore right-to-left and top-to-bottom character because it creases visual inconsistency. 
	   Instead, we just display the string backwards. */
	/* and pretend that top-to-bottom doesn't exist. This behavior may change. (The backwards text unicode character is ignored.) */
	WSTRING LangLong;

	// (one reason for that behavior is to prevent someone using it, for example, like 'someAwesomeTheme for stuff by ExE.theme', where it's actually
	// a disguised executable taking advantage of Unicode support.)

	BOOL IsRightToLeft;	// When this is true, we display the characters of all messages in the reverse order, and when possible, aligned right.
	
}ContentLanguage;

typedef UINT32 MessageBoxType;

// Retain similar (but not identical) to another platform so porting is easier.
// (aka we want to make it so you can just OR a whole bunch together and create a pretty good MessageBox with whatever you want, even if it makes no sense.)
// (im aware the reason is for memory usage, but RAM is cheap for many many many gigabytes and that level of optimization isn't really necessary anymore)
// (might as well get rid of legacy stuff that's no longer helpful while we can.) We care more about battery life and RAM compression than cramming into
// little small structs. Avoiding swapfile, use RAM compression if we need to, only swap if we're _really_ out of space or performance in jeopardy.

#define MB_OK			((MessageBoxType)0x00)
#define MB_CANCEL		((MessageBoxType)0x01)
#define MB_YES			((MessageBoxType)0x02)
#define MB_RETRY		((MessageBoxType)0x04)
#define MB_NO			((MessageBoxType)0x08)
#define MB_IGNORE		((MessageBoxType)0x10)
#define MB_ABORT		((MessageBoxType)0x20)
#define MB_CONTINUE		((MessageBoxType)0x30)

// The window manager (part of the kernel) does special things in this case.
// Yes, the WM is part of the kernel (rather, more accurately, the display server which actually does the rendering)
// The actual manager is still userspace (dynamically linked as part of applications, actually), to improve performance.
// Kind of like Wayland actually. Avoids IPC and relegates draws to the kernel, which is faster than 2 IPC syscalls with something like Xorg (which is worse with a 3d compositor).
#define MB_HELP			((MessageBoxType)0x80)

// And a couple defaults.
#define MB_OKCANCEL		((MessageBoxType)(MB_OK     | MB_CANCEL))
#define MB_RETRYCANCEL		((MessageBoxType)(MB_RETRY  | MB_CANCEL))
#define MB_YESNO		((MessageBoxType)(MB_YES)   | MB_NO))
#define MB_ABORTRETRYIGNORE	((MessageBoxType)(MB_IGNORE | MB_RETRY    | MB_ABORT))
#define MB_CANCELRETRYCONTINUE	((MessageBoxType)(MB_IGNORE | MB_CONTINUE | MB_CANCEL))
#define MB_YESNOCANCEL		((MessageBoxType)(MB_YES)   | MB_NO       | MB_CANCEL))

/* MessageBox Icons (byte 2) */
#define MB_ICONEXCLAMATION	((MessageBoxType)(0x01 << 8))
#define MB_ICONWARNING		((MessageBoxType)(0x01 << 8))
#define MB_ICONINFORMATION	((MessageBoxType)(0x02 << 8))
#define MB_ICONASTERISK		((MessageBoxType)(0x02 << 8))
#define MB_ICONSTOP		((MessageBoxType)(0x04 << 8))
#define MB_ICONERROR		((MessageBoxType)(0x04 << 8))
#define MB_ICONHAND		((MessageBoxType)(0x04 << 8))

/* MessageBox default buttons (byte 3) */
#define MB_DEFBUTTON1		((MessageBoxType)(0x00 << 16))
#define MB_DEFBUTTON2		((MessageBoxType)(0x01 << 16))
#define MB_DEFBUTTON3		((MessageBoxType)(0x02 << 16))
#define MB_DEFBUTTON4		((MessageBoxType)(0x04 << 16))
#define MB_DEFBUTTON5		((MessageBoxType)(0x08 << 16))
#define MB_DEFBUTTON6		((MessageBoxType)(0x10 << 16))
#define MB_DEFBUTTON7		((MessageBoxType)(0x20 << 16))
#define MB_DEFBUTTON8		((MessageBoxType)(0x30 << 16))

// Modality of the messagebox.
#define MB_APPLMODAL		((MessageBoxType)(0x40 << 16))	// Only prevent that thread from continuing.
#define MB_SYSTEMMODAL		((MessageBoxType)(0x50 << 16))	// Warn of serious error (special style)
#define MB_TASKMODAL		((MessageBoxType)(0x60 << 16))	// Something serious has gone wrong and the messagebox needs to halt the whole app.

// This is all we really need (other types don't really have any meaning to us. Use the above. By default, if there is no user logged on,
// we display anyway.)
// We have 1 byte reserved too!

//TODO: Move this gigantic code out into a separate header. Include if and only if when WAYPOINT_LEAN_AND_MEAN isn't defined.

#define WPFRAME_CLOSE_BUTTON             ((UINT8)(1 << 0))
#define WPFRAME_MAXIMIZE_MINIMIZE_BUTTON ((UINT8)(1 << 1))
#define WPFRAME_ICONIFY_BUTTON           ((UINT8)(1 << 2))

typedef UINT8 WPFRAME_HEADER_BUTTONS;

struct WPFRAME;

typedef struct WPFRAME
{

	WP_BASE_STRUCT_MEMBERS;
	
	WSTRING FrameTitle;			// The title of this frame.
	WPFRAME_HEADER_BUTTONS HeaderButtons;	// What buttons does the window manager assign to this frame?

	UINT64 MinFrameSizeX;
	UINT64 MinFrameSizeY;

	// These should be set to ((1 << 64) - 1) if there is no max. Technically, this is "an arbitrary sized number", but is the maximum we can do _anyway_.
	UINT64 MaxFrameSizeX;
	UINT64 MaxFrameSizeY;
	
	UINT64 NumChildren; 	// Up to 2^64 handles to children per application. No application _ever_ should need that amount, or if they do, they need to redesign it.
	WPFRAME* Children;
	
	//TODO
}WPFRAME;

typedef enum MB_RESULT_TYPE
{
	ID_ABORT,	// Recommended practice is to use this when "Cancel" doesn't make any sense (ie, "STOP THIS RIGHT NOW, EVEN IF THAT MEANS MEMORY LEAK".)
	ID_CANCEL,	// and this when it's more of "nicely stop whatever we're doing since we don't care anymore.")
	ID_CONTINUE,
	ID_IGNORE,
	ID_NO,
	ID_YES,
	ID_OK,
	ID_RETRY,
	ID_TRYAGAIN,
	ID_CLOSED,	// User closed the messagebox instead of answering it.
}MB_RESULT_TYPE;

typedef enum WP_FRAME_MSG
{
	WP_MSG_POLL,	// App must do something or else marked as 'Not responding' in this case.
	WP_MSG_CLOSE,	// We're requesting the app to close.
	WP_MSG_MINIMIZE,
	WP_MSG_MAXIMIZE,
	WP_MSG_NORMALSIZE,	// Bring the app to go back to whatever it's desired size is. This should be bigger than (64x64).
	WP_MSG_SYSTEM_SHUTDOWN,	// The system wants to shut down.
	WP_MSG_SYSTEM_HIBERNATE,	// The system wants to hibernate. Do whatever you were doing before, but avoid doing too performance-intensive stuff.
	WP_MSG_SYSTEM_SLEEP,	// The system wants to sleep. Your program will be halted very soon.
	//TODO
}WP_FRAME_MSG;

typedef enum WpStructType
{
	STRUCT_TYPE_WP_GLOBAL_APPLICATION_STATE,
	STRUCT_TYPE_WP_APPLICATION_STATE,
	STRUCT_TYPE_WP_SYSTEM_STATE;
	STRUCT_TYPE_WP_FRAME;
}WpStructType;

typedef struct
{
	//TODO... this is #if 0ed for now just to get the rough idea of what to do.
	// Importantly, we lock applications to picking which peripherals to use. This is so that we can let games choose better, avoid multiple keyboards causing issues (mouse/trackpads!!!)
	// and so on.
	
	WP_BASE_STRUCT_MEMBERS;
#if 0
	UINT64 NumKeyboards;
	KBRDHANDLE* Keyboards;	// Handles to various keyboards. (For me, I have a 1532:010F, from "the other Green team")
	WSTRING* KeyboardNames;	// Driver identifier for the keyboard in use. Under linux, you can see the output with lsusb -v.

	UINT64 NumMice;
	MOUSEHANDLE* Mice;	// For me, I have a 1532:0053.
	WSTRING* MiceName;

	UINT64 NumControllers;
	CNTRHANDLE* Controllers;
	WSTRING* ControllerNames;

	UINT64 NumDisplays;
	DISPLAYHANDLE* Display;
	WSTRING* DisplayNames;

	// In the case of a headset, we create 2 identical entities in speakers and microphones.

	UINT64 NumSpeakers;
	SPEAKERHANDLE* Speakers;
	WSTRING SpeakerNames;
	
	UINT64 NumMicrophones;
	MICHANDLE* Microphones;
	WSTRING* MicrophoneNames;
#endif
}WpSystemState;

typedef struct
{
	WP_BASE_STRUCT_MEMBERS;
	
	COLORREF PreferredForegroundColor;
	COLORREF PreferredBackgroundColor;

	FILE* DefaultCursor;	// Default cursor for this application. By default, set to the system's.
	FILE* DefaultIcon;	// Default icon for this application, by default, set to the system's based on type (game, browser, etc.)

}WpGlobalApplicationState;

typedef struct
{
	WP_BASE_STRUCT_MEMBERS;
	
	COLORREF CurrentForegroundColor;
	COLORREF CurrentBackgroundColor;
	
	FILE* IconRef;
	
	// The main method to call when updating this frame. This changes the event state and all,
	// and is required for all GUI applications.
	VOID (*ApplicationRepaint)(WP_FRAME_MSG MSG);

	
}WpApplicationState;


WPSTATUS CreateApplication(IN WSTRING Title, IN HUSER User, OUT WpApplicationState* State);	// Create an application state.
WPSTATUS ModifyApplicationColors(INOUT WpApplicationState* State, INOPT COLORREF* Foreground, INOPT COLORREF* Background);

WPSTATUS CreateApplicationFrame(IN WpApplicationState* AppState, OUT WPFRAME* Frame);

// We like to use Unicode because not everyone uses the same 26 (52 with uppercase) characters.
MB_RESULT_TYPE MessageBox(INOPT WPFRAME* ParentHandle, INOPT WSTRING MessageString, INOPT WSTRING MessageBoxTitle, IN MessageBoxType Type, ContentLanguage Lang);




#endif