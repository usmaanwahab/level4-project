FROM ubuntu:24.04 AS noelle

RUN apt-get update && apt-get install -y build-essential cmake git llvm-14-dev clang-14 libomp-14-dev zlib1g-dev bash

WORKDIR /opt
RUN git clone https://github.com/arcana-lab/noelle.git

WORKDIR /opt/noelle/build
RUN cmake .. -DCMAKE_INSTALL_PREFIX=/opt/noelle/install && make -j$(nproc)

FROM ubuntu:24.04 AS klee
RUN apt-get update && apt-get install -y cmake git bash zlib1g-dev build-essential z3 libz3-dev clang-14 llvm-14 llvm-14-dev llvm-14-tools

WORKDIR /opt
RUN git clone https://github.com/klee/klee-uclibc.git
RUN git clone https://github.com/klee/klee.git

WORKDIR /opt/klee-uclibc
RUN ./configure --make-llvm-lib --with-cc clang-14 --with-llvm-config llvm-config-14
RUN make -j"$(nproc)"

WORKDIR /opt/klee/build
RUN cmake -DENABLE_POSIX_RUNTIME=ON -DENABLE_SOLVER_Z3=ON -DKLEE_UCLIBC_PATH=/opt/klee-uclibc -DLLVMCC=clang-14 -DLLVMCXX=clang++-14 ..
RUN make -j"$(nproc)"

FROM ubuntu:24.04 
COPY --from=noelle /opt/noelle /opt/noelle
COPY --from=klee /opt /opt

RUN apt-get update && apt-get install -y build-essential cmake llvm-14-tools llvm-14-dev clang-14 libomp-14-dev zlib1g-dev bash z3 libz3-dev
RUN ln -s /usr/bin/opt-14 /usr/bin/opt
RUN ln -s /usr/bin/llvm-symbolizer-14 /usr/bin/llvm-symbolizer
RUN ln -s /usr/bin/clang-14 /usr/bin/clang
RUN ln -s /usr/bin/llvm-profdata-14 /usr/bin/llvm-profdata

ENV PATH="/opt/noelle/install/bin:${PATH}"
ENV KLEE_UCLIBC_PATH=/opt/klee-uclibc
ENV PATH="/opt/klee/build/bin:${PATH}"

WORKDIR /opt/noelle/build
RUN make install
WORKDIR /workspace
