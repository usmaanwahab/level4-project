OpenMP is an API specification for explicit multi-threaded, shared memory parallelism. It has three main components.
- Compiler Directives
- Runtime Library Routines
- Environment Variables

OpenMP is a **specification**. This means that it is not necessarily implemented identically across vendors.
OpenMP programs achieve parallelism through the use of threads. A thread of execution is the smallest unit of processing that can be scheduled by an operation system. Threads are typically spawned by a parent process. If the parent dies, the threads follow.

OpenMP uses a fork-join model of parallel execution. That is all OpenMP programs begin as a single process, the initial thread. This thread executes sequentially until the first parallel region construct is encountered.

**Fork** - The initial thread then creates a team of parallel threads. The statements in the program that are enclosed by the parallel region construct are then executed in parallel among various team threads.

**Join** - When the team threads complete the statements in the parallel region construct, they synchronise and terminate, leaving only the initial thread. The number of parallel regions and the threads that comprise them are arbitrary.

OpenMP specification allows for placement of parallel regions inside other parallel regions. The thread of execution that encounters the nested parallel construct becomes the initial thread of the new team created for that region.

OpenMP follows the **single-program multiple-data** paradigm, where all threads have potential to execute the same program code, but each thread may access/modify different data and/or traverse different execution paths.

OpenMP provides a "relaxed-consistency" and "temporary" view of thread memory - threads have equal access to shared memory where each variable can be retrieved/stored. Each thread also has its own temporary copies of variables that may be modified independent from variables in memory.

OpenMP specifies a number of scoping rules on how directives may associate with each other. 

**Static Extent** - The code is textually between the beginning and the end of a structures block, for example a `do` directive within an enclosing parallel region.

**Orphaned Directive** - An OpenMP directive that appears independently from another enclosing directive is said to be an orphaned directive. It exists outside of another directives static extent.

**Dynamic Extent** - The dynamic extent of a directive includes both its static extend and the extents of its orphaned directives, for example a function that has a critical section is called by another parallel region.