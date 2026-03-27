
**Date & Time:** 12-02-2026 @ 12:30  
Attendees: Usmaan & Nikela

---

Progress

---

Mostly reading for background and introduction of dissertation writing 

Questions

---

So far I mainly just have the information provided by OpenMP through it's PDG,  I believe the next step is to summarise the dependence information per outlined region provided by OpenMP. Is this just as simple as creating a struct which is as compact as possible? The original naive idea I provided at the start was just using some map, instead would it be better to use something like,

```
struct MemoryAccessNode {
    Value* baseAddrPtr;
    bool isRead;
    int stride;
    bool isLoopCarried;
    std::unsorted_set<MemoryAccessNode> dependsOn;
};

struct TaskInfo {
    llvm::Function* outlinedFunction;
    std::vector<MemoryAccessNode> memoryAccesses;
};
```
Once I've created these would the next task be to perform some evaluation? This would be by hand or could we create some kind of simulated thing? 

## Answers / Decisions
- 

---

## Action Items
- [ ] 

---

## Quick Notes (during meeting)

Give each region it's own ID, as the number of threads is variable.
Make an assumption on eight threads.

If stride is not know at compile, hasStride(), isStrideKnow() as stride may be dynamic
Assume the OpenMP program is correct.
Create an entry node in the graph.

Intel PIN memory dependency analysis.
Analyse memory dependencies.

What does noelle miss, how can we improve.

Polybench
Look into barcelona.
