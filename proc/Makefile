RUSTC = rustc
XARGO = cargo
RUSTFLAGS = --target=x86_64-pc-feral-gnu -v

procmgr:
	$(XARGO) xbuild $(RUSTFLAGS) 
	cp target/x86_64-pc-feral-gnu/debug/libprocmgr.a ../

clean:
	$(XARGO) clean
