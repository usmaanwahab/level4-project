Strongly Connected Components Directed Acyclic Graph.

An abstraction that allows us to reason about the dependencies at a higher level than individual instructions.

It is the graph formed by collapsing each SCC into a single node, and keeping edges between SCCs according to their original dependencies.

It is very useful for transformations such as
- parallelisation
- memory locality
- instruction scheduling


