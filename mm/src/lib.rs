#![feature(lang_items)]
#![feature(panic_implementation)]
#![no_std]

#![allow(non_snake_case)]	/* We use camelcase for interacting with the C code. Leave snakecase for rust-specific stuff. */

/* We're expecting to be implementing this function. As such, we must leave it compatible with C. */
#[no_mangle]
pub extern fn KiInitializeProcMgr(){}

/* We should also fill out the rest of these as no_mangle so that we can call them from C. */
pub extern fn KeCreateProcess(){}	//TODO
pub extern fn KeDestroyProcess(){}	//TODO
pub extern fn KeModifyProcess(){}	//TODO

#[lang = "eh_personality"] 
extern fn eh_personality(){}

#[panic_implementation]
#[lang = "panic_impl"] #[no_mangle] 
pub extern fn panic_impl(_info: &core::panic::PanicInfo) -> !{loop{}}
