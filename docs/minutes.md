# 25-09-2025@1100
---
This will be the first meeting with Supervisor (Nikela Papadopoulou).
Questions
- What kind of locality specifically? Temporal/Spacial/Cache/NUMA?
- How detailed should the analysis pass be?
- Paper recommendations?
- What OpenMP constructs and types or parallelism to focus on?
- The goal of this project is analysis and not transformations/optimisations?
- How much proving and maths is required for this? (I don't have a strong maths background)
- Example applications?

From what I can read analysis in to parallelised for-loops is a well researched area even so far as to be in the LLVM project via a project called Polly do you think its reasonable to move focus more towards task based locality tools. 

---
MEETING NOTES
Symbol pointer tracking 

Memory dependency analysis.

Try to get a complete map of memory dependencies of an OpenMP program at compile time.

Find dependencies between for loop chunks and task

AST Level or IR level 

OpenMP scheduling mechanism, maybe start with loo

memory aware topology aware scheduling 

Significant gain energy to avoid memory transfers 
Not looking for much performance gains.

Static analysis is first step.

memory carry dependencies

The case where this chunk depends on some memory used by another chunk 

Group them together into something meaningful

OpenMP runtime supports finding the topology

Last level cache 

Any analysis is better than none

GOAL 1 - map out as much memory dependencies as possible.

GOAL 2 - analyse memory layout and dependencies to move relevant stuff as close as possible

Control Flow Graph in LLVM?

Use school servers instead 

Control flow graph
Memory analysis graph

Getting tons of papers and textbooks and references

Don't need strong maths and will find examples 