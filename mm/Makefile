RUSTC = rustc
XARGO = cargo
CARGO_XBUILD = xbuild
RUSTFLAGS = --target=x86_64-pc-feral-gnu -v --jobs `nproc` -C -j`nproc`

procmgr:
	$(XARGO) $(CARGO_XBUILD) $(RUSTFLAGS) 
	cp target/x86_64-pc-feral-gnu/debug/libmm.a ../

clean:
	$(XARGO) clean
