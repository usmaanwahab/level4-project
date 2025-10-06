FROM ubuntu:24.04 AS builder

RUN apt-get update && apt-get install -y build-essential cmake git llvm-14-dev clang-14 libomp-14-dev bash

WORKDIR /opt
RUN git clone https://github.com/arcana-lab/noelle.git

WORKDIR /opt/noelle/build
RUN cmake .. -DCMAKE_INSTALL_PREFIX=/opt/noelle/install && make -j$(nproc)

FROM ubuntu:24.04 
COPY --from=builder /opt/noelle /opt/noelle
RUN apt-get update && apt-get install -y build-essential cmake llvm-14-dev clang-14 libomp-14-dev bash

WORKDIR /opt/noelle/build
RUN make install
WORKDIR /workspace
