#![feature(lang_items)]
#![no_std]

#![allow(non_snake_case)]	/* We use camelcase for interacting with the C code. Leave snakecase for rust-specific stuff. */

/* We're expecting to be implementing this function. As such, we must leave it compatible with C. */
#[no_mangle]
pub extern fn KiInitializeProcMgr(){}

/* We should also fill out the rest of these as no_mangle so that we can call them from C. */
#[no_mangle]
pub extern "C" fn KeCreateProcess(){}	//TODO
#[no_mangle]
pub extern "C" fn KeDestroyProcess(){}	//TODO
#[no_mangle]
pub extern "C" fn KeModifyProcess(){}	//TODO


/*
	We'll have to provide some way to support *both* user threads and kernel threads.
	Some programs may achieve better performance with either.
 */


///	Definition for a FeralProcess.
///	We define Process ID, Group ID, and Owner ID as UINT64.
///	The C part of the kernel defines these accordingly: Each ID is unique.
struct FeralProcess
{
	ProcId:		u64,
	GroupId:	u64,
	OwnerId:	u64,
}

struct FeralProcessConnector
{
	ProcIdSrc:	u64,
	ProcIdDest:	u64,
}

struct FeralMessageUINT64
{
	Connector:	FeralProcessConnector,
	Number:		u64,
}

struct FeralMessageINT64
{
	Connector:	FeralProcessConnector,
	Number:		i64,
}

struct FeralMessageUINT32
{
	Connector:	FeralProcessConnector,
	Number:		u32,
}

struct FeralMessageINT32
{
	Connector:	FeralProcessConnector,
	Number:		i32,
}

struct FeralMessageUINT16
{
	Connector:	FeralProcessConnector,
	Number:		u16,
}

struct FeralMessageINT16
{
	Connector:	FeralProcessConnector,
	Number:		i16,
}

struct FeralMessageUINT8
{
	Connector:	FeralProcessConnector,
	Number:		u8,
}

struct FeralMessageINT8
{
	Connector:	FeralProcessConnector,
	Number:		i8,
}


pub enum ProcError
{
	NotFound,
	NotExecutable,
	InvalidFormat,
	InvalidArguments,
	CorruptedExecutable,
}

#[lang = "eh_personality"] 
extern fn eh_personality(){}

#[lang = "panic_fmt"] #[no_mangle] 
pub extern fn panic_fmt() -> !{loop{}}
