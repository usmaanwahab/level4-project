# level4-project

Exploring Static Analysis Techniques to Enhance Memory Locality in OpenMP Parallelism

Effective memory locality is essential for optimising performance in parallel computing environments using OpenMP. This project focuses on developing and applying static analysis techniques to identify memory dependencies between OpenMP tasks. The primary goal is to enhance the compile-time understanding of how data is accessed and shared across parallel tasks, thereby providing insights into memory access patterns and potential improvements in locality.

Objectives:
- To leverage LLVM static analysis capabilities to identify and model memory dependencies in OpenMP tasks and parallelised for-loops
- To implement custom LLVM compiler passes to extend the static analysis capabilities, focusing on detecting memory locality issues
