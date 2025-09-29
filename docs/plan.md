So I want to perform static analysis on an intermediate representation of (for now) a `c++` source file.

It was also recommended to perform some analysis over the **Abstract Syntax Tree**.

Specifically I want to model memory dependencies in `c++` code that takes advantage of the OpenMP runtime.

I want to statically (and possibly dynamically) find memory dependencies in OpenMP `#pragma omp for` loops. This typically uses a static scheduling policy.

Then, I want to statically (and probably dynamically) find memory dependencies in OpenMP `tasks`. This typically uses a dynamic scheduling policy.

I need to look into the techniques used to find different kinds of dependencies I assume it would be better too look into **Read After Write** dependencies since these cause the most problems as **Write After Read** and **Write After Write** are typically solvable or can be at least mitigated by using renaming and some **Reorder Buffer** and **Out of Order** execution mechanism.
