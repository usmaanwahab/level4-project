1. If you require inter-procedural analysis, it should be a `pass`
2. If you are a function-local analysis, subclass `FunctionPass`
3. If you don't need to look at the program at all, subclass `ImmutablePass`
