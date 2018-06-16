#!/bin/sh


## Not exactly sure how Darwin's directory structure works (Not everything is shoved into /usr/bin?? ???????)
## As such, homebrew is very helpful here.

## Darwin doesn't come with clang-lld, and I don't feel like installing it on my Darwin PC. (i mean it's an x86 and it runs Darwin so yeah it's a Darwin PC with bizzare firmware.)
## you'll need that to start building the kernel. Probably won't work anyway.

brew install rustup
brew install xorriso
brew install bmake
brew install yasm

rustup-init

rustup override add nightly
rustup component add rust-src

## Errors __ARE__ expected here, as Darwin's version of rustup is slightly different.
## The above commands prevent the errors from breaking the build.
./setup.sh
