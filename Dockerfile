FROM ubuntu:24.04 AS builder

RUN apt-get update && apt-get install -y build-essential cmake git llvm-14-dev clang-14 libomp-14-dev zlib1g-dev bash

WORKDIR /opt
RUN git clone https://github.com/arcana-lab/noelle.git

WORKDIR /opt/noelle/build
RUN cmake .. -DCMAKE_INSTALL_PREFIX=/workspace/noelle/install && make -j4
WORKDIR /opt/noelle/build
RUN make install

FROM ubuntu:24.04
WORKDIR /workspace
COPY --from=builder /opt/noelle /opt/noelle
RUN apt-get update && apt-get install -y build-essential cmake llvm-14-dev clang-14 libomp-14-dev zlib1g-dev bash
RUN ln -s /usr/bin/opt-14 /usr/bin/opt
RUN ln -s /usr/bin/llvm-symbolizer-14 /usr/bin/llvm-symbolizer
RUN ln -s /usr/bin/clang-14 /usr/bin/clang
RUN ln -s /usr/bin/llvm-profdata-14 /usr/bin/llvm-profdata
ENV PATH="/opt/noelle/install/bin:${PATH}"


WORKDIR /workspace
