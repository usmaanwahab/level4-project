#!/bin/bash
set -e

SRCDIR="/workspace/bots/omp-tasks/sparselu/sparselu_single"
COMMON_DIR="/workspace/bots/common"
OUT="/workspace/build"
BASE="sparselu_single"
PASS="${OUT}/MyPass.so"
INCLUDES="-I${COMMON_DIR} -I${SRCDIR}"

clang-14 -O2 -fopenmp ${INCLUDES} \
  ${SRCDIR}/sparselu.c \
  ${COMMON_DIR}/bots_main.c \
  ${COMMON_DIR}/bots_common.c \
  -o ${OUT}/${BASE}-normal

clang-14 -O2 -fopenmp -emit-llvm -c ${SRCDIR}/sparselu.c ${INCLUDES} -o ${OUT}/sparselu.bc
clang-14 -O2 -fopenmp -emit-llvm -c ${COMMON_DIR}/bots_main.c ${INCLUDES} -o ${OUT}/bots_main.bc
clang-14 -O2 -fopenmp -emit-llvm -c ${COMMON_DIR}/bots_common.c ${INCLUDES} -o ${OUT}/bots_common.bc

llvm-link-14 ${OUT}/sparselu.bc ${OUT}/bots_main.bc ${OUT}/bots_common.bc -o ${OUT}/${BASE}.bc

noelle-norm ${OUT}/${BASE}.bc -o ${OUT}/${BASE}-norm.bc
noelle-load -load ${PASS} -OpenMPDependence ${OUT}/${BASE}-norm.bc -o ${OUT}/${BASE}-embedded.bc

llc-14 -opaque-pointers -relocation-model=pic -filetype=obj ${OUT}/${BASE}-embedded.bc -o ${OUT}/${BASE}-embedded.o
clang-14 -fopenmp -fPIE ${OUT}/${BASE}-embedded.o -o ${OUT}/${BASE}-noelle

llvm-dis-14 -opaque-pointers ${OUT}/${BASE}-embedded.bc -o ${OUT}/${BASE}-embedded.ll

ls -lh ${OUT}/${BASE}*
