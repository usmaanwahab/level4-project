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

A **parallel** region is a block of code that will be executed by multiple threads. This is the fundamental OpenMP parallel construct.

```c
#pragma omp parallel [clause ...]
{
 // structured block
}
```

Clauses can include:

- `if(scalar_expression)` — conditionally enable or disable parallelism
- `private(list)` — each thread gets its own uninitialised copy of the variables
- `shared(list)` — variables are shared by all threads in the team
- `default(shared | none)` — sets the default data-sharing attribute for unspecified variables
- `firstprivate(list)` — each thread gets a private copy, initialised with the value from the master thread
- `reduction(operator: list)` — perform a reduction across all threads using the specified operator
- `copyin(list)` — initialise `threadprivate` variables from the master’s value
- `num_threads(integer-expression)` — request a specific number of threads for this region


When a thread reaches a `parallel` directive, it creates a team of threads and becomes the initial thread for that team. The initial thread is also part of the working threads. Starting from the beginning of the parallel regions, the code is duplicated and all threads will execute that code. 

There is an **implied** barrier at the end of the parallel section. Only the initial thread continues execution past this point. If any threads terminates within a parallel region, all threads in the team will terminate, and the work done up until that point is undefined.

The number of threads in a parallel region is determined by the following conditions, in order:
- Evaluation of the `IF` clause
- Setting of the `NUM_THREADS` clause
- Use of the `omp_set_num_threads()` library function
- Setting of `OMP_NUM_THREADS` environment variable
- Implementation default - usually number of CPUs on a node.

Dynamic threads are important for nested parallelism. If nested parallelism is **disabled** or dynamic teams are **not supported**, a parallel region nested inside another parallel region executes **serially**, with only the encountering thread acting as the team. If nested parallelism and dynamic threads are **enabled**, the runtime can create a new team of threads for the inner region, up to the number specified by `num_threads`.

Restrictions on parallel constructs: 
- A parallel region must be a structured block that does no span multiple routines or code files
- It is illegal to branch (`goto`) into or out of a parallel region
- Only a single `IF` clause is permitted
- Only a single `NUM_THREADS` clause is permitted
- A program must no depend upon the ordering of the clauses