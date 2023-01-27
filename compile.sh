#!/bin/bash

baseDir=$(pwd)

echo "Compiling libnx ..."
cd lib/libnx
make -j4 && make install

if [ $? != 0 ]
then
	cd "$baseDir"
	exit 1
fi

echo
echo "Compiling Atmosphere libs ..."
cd ../Atmosphere-libs/libstratosphere
make

if [ $? != 0 ]
then
	cd "$baseDir"
	exit 2
fi

echo
echo "Generating .zip ..."
cd ../../..
make dist

if [ $? != 0 ]
then
	cd "$baseDir"
	exit 3
fi

echo
echo "Compilation complete"

