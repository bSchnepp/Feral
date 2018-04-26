#!/bin/sh


## Not exactly sure how Darwin's directory structure works (Not everything is shoved into /usr/bin?? ???????)
## As such, homebrew is very helpful here.

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
