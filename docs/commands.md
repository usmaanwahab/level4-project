```bash
# install required packages
sudo apt install llvm-14 clang-14 lldb-14 lld-14
sudo apt install llvm-20 clang-20 lldb-20 lld-20

# make the make
cmake .. -DLLVM_DIR=/usr/lib/llvm-20/lib/cmake/llvm
cmake .. -DLLVM_DIR=/usr/lib/llvm-14/lib/cmake/llvm


# compile to LLVM IR
clang -fopenmp -S -emit-llvm main.c -o main.ll

# run custom pass on pre-made IR (from build dir)
opt-14 -load-pass-plugin=./HelloWorldPass.so -passes=hello-world ../tests/test1.ll -disable-output

opt-20 -load-pass-plugin=./HelloWorldPass.so -passes=hello-world ../tests/test1.ll -disable-output
noelle-load -load ./HelloWorldPass.so -CAT ../tests/test2.bc -o ../tests/test2-out.bc
```
