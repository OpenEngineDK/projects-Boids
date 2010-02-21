#!/bin/sh

export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:libraries/WiiRemote.framework
./build/Boids/Boids

