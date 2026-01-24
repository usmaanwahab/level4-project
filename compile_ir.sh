#! /bin/bash

DIR="${1:-.}"

for f in "$DIR"/*.cpp; do
  [ -e "$f" ] || continue

  base="$(basename "${f%.cpp}")"
  outdir="$(dirname "$f")"

  clang++-14 -fopenmp -emit-llvm -S "$f" -o "$outdir/${base}.ll"
  clang++-14 -fopenmp -emit-llvm -c "$f" -o "$outdir/${base}.bc"

done
