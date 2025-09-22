```bash
# install required packages
sudo pacman -S clang llvm lldb lld
yay -S openmp

# compile to LLVM IR
clang -fopenmp -S -emit-llvm main.c -o main.ll

# run custom pass on pre-made IR (from build dir)
opt -load-pass-plugin=./HelloWorldPass.so -passes=hello-world ../tests/test1.ll -disable-output
```
