Static single assignment is a type of intermediate representation where each variable is assigned only once. 

The primary benefit of SSA is that is simplifies and improves the results of a variety of compiler optimisations, by reducing the complexity in the properties of variables.

```c
y = 1
y = 2
x = y
```

Trivially, we can see that the first assignment is not necessary and can be removed completely without changing the semantics of the program. A program would have to perform [[Reaching Definition Analysis]] to determine this. 

However, if the program is in SSA form, both of these are immediate.
```c
%0 = 1
%1 = 2
%2 = %1
```

Compiler optimisations are either enabled or strongly enhanced by SSA
- [[Constant Folding]]
- [[Value Ranged Propagation]]
- [[Sparse Conditional Constant Propagation]]
- [[Dead-code Elimination]]
- [[Global Value Numbering]]
- [[Partial-redundancy Elimination]]
- [[Strength Reduction]]
- [[Register Allocation]]
