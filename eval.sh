#!/bin/bash

CPP_FILE="$1"
BASE="$(basename "${CPP_FILE%.cpp}")"

clang++-14 -O2 -fopenmp "$CPP_FILE" -o "${BASE}-normal"

clang++-14 -O2 -fopenmp -emit-llvm -c "$CPP_FILE" -o "${BASE}.bc"

noelle-norm "${BASE}.bc" -o "${BASE}-norm.bc"
noelle-load -load ./MyPass.so -OpenMPDependence "${BASE}-norm.bc" -o "${BASE}-embedded.bc"

llc-14 -opaque-pointers -relocation-model=pic -filetype=obj "${BASE}-embedded.bc" -o "${BASE}-embedded.o"

clang++-14 -fopenmp "${BASE}-embedded.o" -o "${BASE}-noelle"

clang++-14 -fopenmp -no-pie "${BASE}-embedded.o" -o "${BASE}-nopie"

llvm-dis-14 -opaque-pointers "${BASE}-embedded.bc" -o "${BASE}-embedded.ll"
