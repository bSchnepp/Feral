RUSTC = rustc
XARGO = cargo
RUSTFLAGS = --target=x86_64-pc-feral-gnu -v

procmgr:
	$(XARGO) xbuild $(RUSTFLAGS) 
	cp target/x86_64-pc-feral-gnu/debug/libmm.a ../

clean:
	$(XARGO) clean
