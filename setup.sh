#!/bin/sh
rustup override add nightly
rustup component add rust-src
##cargo install xargo --force
##sudo ln -sf ~/.cargo/bin/xargo /usr/bin/xargo
cargo install cargo-xbuild
