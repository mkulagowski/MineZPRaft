#!/bin/bash

pushd . > /dev/null
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd ${DIR}/..
echo -n "Current directory is "; pwd

# remove Visual Studio files
echo -n "Removing VS-related files... "
rm -f *.sdf *.vsp *.psess
rm -rf ipch
echo "DONE"

echo -n "Removing compilation results... "
rm -rf Bin
rm -rf Obj
echo "DONE"

echo -n "Removing CMake files... "
Scripts/clean-cmake.sh > /dev/null
echo "DONE"

echo -n "Removing other files... "
# Place here any other non-usual files which need removal...
echo "DONE"

popd > /dev/null
