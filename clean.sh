#! /bin/bash

DIR="${1:-.}"

for f in "$DIR"/*.bc; do
  [ -e "$f" ] || continue

  base="$(basename "${f%.bc}")"
  outdir="$(dirname "$f")"
  rm *.svg
  rm *.dot
  rm *.bc
  rm *.ll
done
