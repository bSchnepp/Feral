#!/bin/sh
cargo install xargo --force
rustup component add rust-src
sudo ln -s ~/.cargo/bin/xargo /usr/bin/xargo
