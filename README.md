# level4-project

Exploring Static Analysis Techniques to Enhance Memory Locality in OpenMP Parallelism

Effective memory locality is essential for optimising performance in parallel computing environments using OpenMP. This project focuses on developing and applying static analysis techniques to identify memory dependencies between OpenMP tasks. The primary goal is to enhance the compile-time understanding of how data is accessed and shared across parallel tasks, thereby providing insights into memory access patterns and potential improvements in locality.

Objectives:
- To leverage LLVM static analysis capabilities to identify and model memory dependencies in OpenMP tasks and parallelised for-loops
- To implement custom LLVM compiler passes to extend the static analysis capabilities, focusing on detecting memory locality issues

### Overview

This project is for a custom LLVM pass that applies static analyis over a given OpenMP source program, and embedd the data in the finall executable.
The project primarily consists of the Docker environment and the main.cpp where the pass is located.

## Build instructions

```
docker compose up --build
```

### Running Pass

```
docker exec -it level4-project-main-1 bash
cp eval.sh build/
cp bots-eval.sh build/

sh eval.sh ../tests/jacobian-1d.cpp # specify the source cpp file where the jacobian path is
sh bots-eval.sh
```

`eval.sh` will generate the executable and files used for inspection during the evaluation, as well as automatically apply the pass and output "source-filename-normal" executable.
`bots-eval` does the same but it requires extra linking


### Requirements

This project mainly uses LLVM14, all necessary dependencies are resolved through docker. Including the developing and linking of NOELLE.
