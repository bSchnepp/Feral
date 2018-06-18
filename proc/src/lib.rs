#![feature(lang_items)]
#![feature(panic_implementation)]
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

pub enum ServiceProviderExecutableFormat
{
	PE,	// useful for WINE and whatnot
	COFF,	// classic *NIXes
	AOUT,	// Very old *NIX
	ELF,	// Pretty much everybody today.
	MACHO,	// Mach derivatives still around (xMach(?), OpenMach, GNU Mach, etc.)
	COM,	// Very old DOS
	MZ,	// Very old DOS
	NE,	// very old DOS
	PE32,
	OTHER,	// Other format (ie, custom)
	NONE,	// Bare executables.
	JVMBYTECODE,	// JVM as a service provider if anyone bothers.
	CLRBYTECODE,	// Mono as a service provider if anyone bothers.
}

// Some programs may actually be compiled for a different kind of CPU (ie, POWER or RISC-V)
// One of the defining features for Feral is "compatibility (in theory) for literally everything, so here we go..."
pub enum ServiceProviderSystemArchitecture
{
	HOST_PLATFORM,		// This provider is for the same as the actual host.
	I8080_GENERIC,		// Some (common) computers had/have pre-x86 derivatives...
	X8632_GENERIC,
	X8632_COPPERMINE,
	X8632_386SX,
	X8664_GENERIC,
	X8664_JAGUAR,
	X8664_BULLDOZER,	// Bulldozer is very, very strange. Some programs like Bulldozer. (ie, compatibility with 3DNow!)
	X8664_PENRYN,
	X8664_HASWELL,
	X8664_ZEN_ONE,		// We can't distinguish Zen 1 from Zen+, other than one is *really good* and the other is *absolutely amazing*.
	RISCV_GENERIC,
	RISCV_FREEDOM_SOC,
	RISCV_RESERVED,			// Expect *a lot* of RISC-V variants out there.
	RISCV_RESERVED2,
	RISCV_RESERVED3,
	RISCV_RESERVED4,
	RISCV_RESERVED5,
	RISCV_RESERVED6,
	RISCV_RESERVED7,
	RISCV_RESERVED8,
	RISCV_RESERVED9,
	RISCV_RESERVED10,
	RISCV_RESERVED11,
	RISCV_RESERVED12,
	RISCV_RESERVED13,
	RISCV_RESERVED14,
	RISCV_RESERVED15,
	RISCV_RESERVED16,
	POWER_GENERIC,
	POWER_DOLPHIN,		// Power "Gekko"
	POWER_REVOLUTION,	// POWER "Broadway"
	POWER_ESPRESSO,		// POWER "Espresso"
	POWER_XCPU,		// POWER "Xenon"
	POWER_XCPU_CELL,	// (ok, we have this backwards, Xenon is CELL PPE + some other stuff, but they're different enough to cause issues.)
	SPARC_GENERIC,
	SPARC_RESERVED,
	SPARC_RESERVED2,
	SPARC_RESERVED3,
	SPARC_RESERVED4,
	AARCH32_GENERIC,
	AARCH32_VERSION_4_GENERIC,
	AARCH32_VERSION_5_GENERIC,
	AARCH32_VERSION_6_GENERIC,
	AARCH32_VERSION_7_GENERIC,
	AARCH32_PROJECT_ATLANTIS,
	AARCH32_NEMESIS_KAL_EL,
	AARCH64_GENERIC,
	AARCH64_VERSION_8_GENERIC,
	AARCH64_NEMESIS_GENERIC,
	AARCH64_NEMESIS_LOGAN,
	AARCH64_NEMESIS_ERISTA,		// "quad"-core Aarch64 SoC with Nemesis Maxwell graphics (256 SIMD "cores" (probably more like 16 compute units)).
	AARCH64_NEMESIS_XAVIER,		// it'll eventually come out...
	MIPS_GENERIC,
	MIPS_R3000,
	MIPS_R4000,
	MIPS_R4200,
	MIPS_R5900,
	MOS_6052C,
	Z80_GENERIC,
	M68K_GENERIC,
	JCORE_GENERIC,
	JCORE_SH2_COMPATIBLE,
	JCORE_SH4_COMPATIBLE,
}


// Create a spot for an external provider to handle process management...
#[no_mangle]
pub extern "C" fn SrvcAttachHandler(Format: ServiceProviderExecutableFormat, Architecture: ServiceProviderSystemArchitecture){}



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

#[panic_implementation]
#[lang = "panic_impl"] #[no_mangle] 
pub extern fn panic_impl(_info: &core::panic::PanicInfo) -> !{loop{}}
