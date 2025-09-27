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

To get started with the project, I should look into mapping out as many memory dependencies as possible.

Then I can move onto analysing these dependencies and memory layout to place relevant computations and data closer together.

I should look into symbol pointer tracking. 
Any analysis is better than none.
Find dependencies between `parallel for` chunks and (dependent?) tasks.

I need to do some analysis on the AST level and also the IR level.

Gain a better understanding of OpenMP scheduling mechanisms, start with loops.

The aim of the project would shift towards saving energy and cycles from DRAM access. Not necessarily meant to get large performance gains.

OpenMP provides details into memory aware topology aware scheduling.

Look into Control Flow Graph, Memory Analysis Graph.

Polybench C programs

Don't need any crazy maths.

Will gain access to school computers for experimenting.

Only Last Level Cache is shared between cores.
