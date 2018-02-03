RUSTC = rustc
XARGO = xargo
RUSTFLAGS = --target=x86_64-pc-feral-gnu -v

procmgr:
	$(XARGO) build $(RUSTFLAGS) 
	cp target/x86_64-pc-feral-gnu/debug/libmm.a ../

clean:
	$(XARGO) clean
