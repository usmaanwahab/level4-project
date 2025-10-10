This is the interface that various alias analysis implementations should support. This class exports two important enums:
- `AliasResult`
- `ModRefResult`
Both of which represent the result of an alias query or a mod/ref query respectively.

Memory objects are represented as a starting address and size, and function calls are represented as the actual call or invoke instructions that perform the call. 

LLVM represents the starting address in IR as a symbolic `LLVM Value*`.

The `getModRefInfo` method returns information about whether the execution of an instruction can read or modify a memory location. This is conservative, such that if an instruction **might** read or write to a location, `ModRef` is returned.

`doesNotAccessMemory` - returns true for a function if the analysis can prove that the function never read or writes to memory, or if the function only reads from constant memory.

`onlyReadsMemory` - returns true for a function if it only reads from non-volatile memory.

Function where either `doesNotAccessMemory` or `onlyReadsMemory` returns true