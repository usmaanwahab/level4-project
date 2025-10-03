#! /bin/bash

rm -rf llvm14/build
mkdir llvm14/build
cd llvm14/build
cmake ..
make
cd ../../
rm -rf llvm20/build
mkdir llvm20/build
cd llvm20/build
cmake ..
make
