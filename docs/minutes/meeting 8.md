
**Date & Time:** 19-11-2025 @ 11:30  
Attendees: Usmaan & Nikela

---

## Progress
- Program dependency graph is extremely useful
- Sent email to publisher of Noelle, on some basic questions about how SVF is utilised in Noelle
- Looking into how to interpret the program dependency graph to categorise dependencies and highlight areas for improvement
-  I plan to look into LLVM20, the following few weeks, specifically focusing on new memorySSA and other features 

---

## Questions
- The current goal is to analyse some code using the PDG and simply recommend (through compile warnings or other methods) that there is room for improvement in terms of memory analysis, is that a reasonable outcome for this dissertation or will it need to be more substantial?
- Should the recommendations be concrete and actionable consider privatising this array or more something like “memory dependence detected here may affect locality"?
- In no circumstance are we to manipulate the existing directives include adding undefined attributes?
---

## Answers / Decisions
- 

---

## Action Items
- [ ] 

---

## Quick Notes (during meeting)

Symbolic exectuon
replace mem adress and check if another isntruction touche it 

shadow heap, see allocations (unsure)

Annotation to openMP overload existing openMP functions

Take information from pragams and directives, wrap existing overloading arguements, extending the runtime don't expos information to user 

Focus openMP programs 

Run PDG on benchmarks 