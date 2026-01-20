**Date & Time:** 22-10-2025 @ 11:30  
Attendees: Usmaan & Nikela

---

## Progress
- Figured out how to use the data flow engine 
- I now need to find the data flow equations that can check if something overlaps
- As I flagged during [[meeting 4]] it seems that NOELLE/LLVM cannot understand or reason about what happens when the code is "lowered" into OpenMP because the library is pre-compiled, but this shouldn't be a problem like you said
- blocker - interview prep for two interviews on 21/10/2025
- blocker - coursework all opening and due over the next two weeks. 
---

## Questions
- 

---

## Answers / Decisions
- 

---

## Action Items
- [ ] Find data flow equations for overlapping memory.
- [ ] Investigate memdeps more.
- [ ] Need to consider at some point to automatically add annotations where possibe?

---

## Quick Notes (during meeting)

NOELLE and LLVM can reason about parallel regions because the directives outline function for parallel regions.

Annotate the IR for data lines that overlap and pin certain tasks to certain cores.

Can see private and shared values, as we can do more reliable DFA on that.

Try and work consistently to develop ground work for winter break and next semester.

How will DFA work on parallel calls, i.e. nodes in the DFA graph at the same level. LOOK INTO THIS.

Implement OpenMP semantics in data flow analysis 

Memdeps in LLVM