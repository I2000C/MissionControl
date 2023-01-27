#!/bin/bash

cd lib/libnx
make clean

cd ../Atmosphere-libs/libstratosphere
make clean

cd ../..
make clean

