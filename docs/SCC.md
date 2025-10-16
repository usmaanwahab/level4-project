Strongly Connected Component

A maximal set of nodes (instructions) in a direct graph where every node is reachable from every other node. The path to each other not need be direct.

An SCC often corresponds to cycles in the graph (loop-carried dependencies, mutual recursion).

In compiler analysis, an SCC represents a unit of instructions that cannot be reordered across nodes in the SCC