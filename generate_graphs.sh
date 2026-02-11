#! /usr/bin/bash

DIR="${1:-.}"

# clean
rm *.svg
rm *.dot
rm *.bc
rm *.ll

# compile ll and bc
for f in "$DIR"/*.cpp; do
  [ -e "$f" ] || continue

  base="$(basename "${f%.cpp}")"
  outdir="$(dirname "$f")"

  clang++-14 -fopenmp -emit-llvm -S "$f" -o "$outdir/${base}.ll"
  clang++-14 -fopenmp -emit-llvm -c "$f" -o "$outdir/${base}.bc"

done

# generate svg
for f in "$DIR"/*.bc; do
  [ -e "$f" ] || continue

  base="$(basename "${f%.bc}")"
  outdir="$(dirname "$f")"
  norm="$outdir/${base}_norm.bc"
  noelle-norm "$f" -o "$norm"
  noelle-ldg-dot $norm --ldg-dot-loop-id=0 
  mv ldg_id_0.dot $norm.dot
  dot -Kneato -Gordering=out -Goverlap=false -Gsplines=ortho -Gnodesep=0.6 -Granksep=1.0 -Tsvg "$norm.dot" -o "${base}.svg"

done

for loop in {0..3}; do
  noelle-ldg-dot jacobian-1d_norm.bc --ldg-dot-loop-id=$loop && \
  dot -Kneato -Gordering=out -Goverlap=false -Gsplines=ortho -Gnodesep=0.6 -Granksep=1.0 -Tsvg ldg_id_$loop.dot -o "jacobian-1d-$loop.svg"
done

# for f in "$DIR"/*_norm.bc; do
#   base="$(basename "${f%.bc}")"
#   outdir="$(dirname "$f")"
#   noelle-ldg-dot $f --ldg-dot-loop-id=0
#   dot -Tsvg "ldg_id_0.dot" -o "${base}.svg"
# done