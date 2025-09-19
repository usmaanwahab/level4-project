```bash
# install required packages
sudo pacman -S clang llvm lldb lld
yay -S openmp

# compile to LLVM IR
clang -fopenmp -S -emit-llvm main.c -o main.ll
```
