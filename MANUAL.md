Rodina Benchmark Download: https://www.cs.virginia.edu/~skadron/lava/rodinia/downloads.html
Intel PIN: https://www.intel.com/content/www/us/en/developer/articles/tool/pin-a-binary-instrumentation-tool-downloads.html
Barcelona Task Suite: https://github.com/bsc-pm/bots

## Evaluation

Compile via the commands in the README

OMP_NUM_THREADS=8 /workspace/pin/pin -t /workspace/pin/source/tools/MemTrace/obj-intel64/memtrace.so -emit 1 -- /workspace/build/jacobian-1d-nopie

OMP_NUM_THREADS=8 /workspace/pin/pin -t /workspace/pin/source/tools/MemTrace/obj-intel64/memtrace.so -emit 1 -- /workspace/build/hotspot-nopie 64 64 2 8 /workspace/rodinia_3.1/data/hotspot/temp_64 /workspace/rodinia_3.1/data/hotspot/power_64 /tmp/hotspot_output.txt

OMP_NUM_THREADS=8 /workspace/pin/pin -t /workspace/pin/source/tools/MemTrace/obj-intel64/memtrace.so -emit 1 -- /workspace/build/sparselu_single-nopie -n 3 -m 32

### Analysis

The memtrace will create several thread log files, and from there we can execute the `python3 instruction_decoder.py` which will perform the aggregation.

This will produce several csv files, which can characterise memory accessess and make it easy to cross-reference data with the source program.