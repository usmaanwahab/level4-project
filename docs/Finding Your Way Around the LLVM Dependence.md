A YouTube video buy Stefanos Baziotis and Simon Molls.

Typically a compiler will try and optimise code by reordering instructions, the most common case of this being executing them in parallel.

In reordering transformations, preserve the dependence of the program is equivalent to preserving the semantics.

Dependence Analysis is the act of gathering information about dependence of a program.

Read-After-Write
```c
int x = 2;
int y = 3;
int c = x  * y;
```

In the above example if we trying and reorder the order of instructions, it will not preserve the semantics of the program, we can reorder line 1 and line 2, but we cannot reorder line 3 with either line 1 or line 2.

We are reading the value of a variable after we've explicitly wrote to it.

Write-After-Read
```c
// x == 10
int y = x * 2;
x = 3;
```

Clearly, we can see that we cannot reorder line 1 and 2 as we assert that `x==10` before we perform the multiplication operation. Reordering will not preserve the semantics of the program.

Write-After-Write
```c
int x = 10;
x = 20;
int c = x * 2;
```

We can see that if we reorder the instructions here we will not preserve the semantics of the programs. However, if there is never a read then it may be possible to reorder the instructions, however doing two writes to the same memory location in parallel will result in undefined behaviour.

A dependence is an ordering between two operations that we have to preserve. Not preserving ordering will result in a read somewhere that will not behave as expected.

A data dependence exists because two operations access the same memory.

```llvm IR
%x = add %a, %b
%y = mul %x, %c
%z = sub %x, %y
```

Below we show how you can query the users of a SSA in a [[Def-User Chain]].
That is what SSAs use `%x`.
```c
llvm::Value *X = /* %x */;
for (auto *User : X->users()) {
	// ...
}
// %y %z
```

Below we show how you can query what an SSA depends on.
```c
llvm::Value *Z = /* %z */;
for (auto *Op: Z->operands()) {
	// ..
}
// %x %y
```