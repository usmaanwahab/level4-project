Cheap reasoning  of various memory operations
`MemorySSA` is better than `MemoryDepdencyAnalayis` in [[LLVM]]
One common use of `MemorySSA` is to quickly find out that something absolutely cannot happen (for example, reason that hoist out of a loop can't happen).

`MemorySSA` is intra-procedural.

- `MemoryDef` - may modify, or ordering constraint
- `MemoryPhi` - if at any-point we have two or more `MemoryDef` that could flow into a BB, the block's top `MemoryAccess` will be `MemoryPhi`
- `MemoryUses` - operations which use but don't modify memory

A `PhiNode` is a pseudo-instruction used in SSA to select a value depending on which path was taken at different control-flow constructs.

`liveOnEntry` implies that the memory being used is either undefined or defined before the function begins. It is used as a "fake" `memorySSA` to denote that the subsequent instructions are using memory that isn't necessarily defined in the function.

If a `MemoryAccess` is a clobber of another, it means that these two `MemoryAccesses` may access the same memory.

`MemorySSAWalker` assists the `MemorySSA`. The goal of the walker is to provide answers to clobber queries beyond what's represented directly by `MemoryAccesses`.