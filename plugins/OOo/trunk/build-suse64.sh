#!/bin/bash
export OO_SDK_HOME=/usr/lib64/ooo3/solver
export OO_SDK_URE_HOME=/usr/lib64/ooo3/basis-link/ure-link
export OFFICE_BASE_PROGRAM_PATH=/usr/lib64/ooo3/basis-link/program
export OO_SDK_URE_LIB_DIR=/usr/lib64/ooo3/basis-link/ure-link/lib
export LD_LIBRARY_PATH=/usr/lib64/ooo3/basis-link/ure-link/lib
export OO_SDK_INC_SUSE=/usr/lib64/ooo3/solver/inc
export OO_SDK_MAP_SUSE=/usr/lib64/ooo3/solenv/src/component.map
cd build
rm -rf *
cmake ..
make
cd ..
