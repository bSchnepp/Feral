#!/bin/sh
rustup override add nightly --force
rustup component add rust-src --force
cargo install xargo --force
sudo ln -sf ~/.cargo/bin/xargo /usr/bin/xargo
cargo install cargo-xbuild
