
**Date & Time:** 08-12-2025 @ 11:30  
Attendees: Usmaan & Nikela

---

## Progress
- klee (symbolic execution engine) built and integrated into the build, ensured that klee would be compatible with external libraries like openmp through build options
- currently I have the memory dependency information provided the NOELLE PDG, hopefully I get some information from klee that I can use to derive more useful insights.
---

## Questions
- the idea with klee the symbolic execution engine is to run the program and explore EVERYTHING and get an understanding of the program and then feed that to the openMP runtime?
- klee seems to be at a significnat cost, is that fine do we care how expensive the analyses is?
_ what should I be looking for explicitly from klee, I am unsure exactly why this expsneive symbolic exuection actually is aimed at providing as from the paper I read it seems to be more used in software testing? Does that indicate that klee is not  the more ideal for this use case should I investigate more symbolic execution tools?
- how significant is the work to bridge the gap between my anlayses and the open mp runtime it doesn't look like there is an easy way of doing this, I can't find any apis or plugin based things online, so it looks like manually configure and compile.
- To clarify again from one of our previus meetings we obviously can't remove directive attributes from the openmp directives BUT we can add more based on information agined from noelle and potentially klee, but is this maybe a fallback if full runtime support is not feasible.
- Is the fisherman server available over the break as I assumed it was fine to run my benchmarks and get metrics from that?
- Availability over teams during the period 12/12/2025 - 12/01/2026, i.e I probably won't be in Glasgow much but I would likely send messages semi regularly for questions
---
## Answers / Decisions
- 



---

## Action Items
- over the holiday break I want to get all the input API correct, i.e. how and what information I can provide OpenMP to impprove the runtime, I don't know how feasible amending the openMP runtime is then as that seems a considerable task.
- will also get my baseline metrics given the epcc openmp benchmark v2 and one or two others to evaluate assuming the runtime implementation works
---

## Quick Notes (during meeting)
extend the kopm struct
annotate the IR

Debug mode openmp

analyse each outline node and find each potiner and memory and heap

not necesary for runtime MVP si filtered info before openMP rune
