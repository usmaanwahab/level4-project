 
**Date & Time:** 08-10-2025 @ 11:30  
Attendees: Usmaan & Nikela

---

## Progress
- Had to move over the Ubuntu to install LLVM 14 and 20
- Reformatted repo to separate LLVM code and LLVM 20 in case of discrepancy later i.e opaque pointers
- Built and linked successfully OpenMP on release 14 for llvm
- Need to learn and play with static analysis techniques

---

## Questions
- 

---

## Answers / Decisions
- 

---

## Action Items
- [x] Investigate [[LLVM]]20 [[MemorySSA]]
- [ ] Watch LLVM 2020 YouTube video

---

## Quick Notes (during meeting)

LLVM20 allows memory dependency analysis over function perhaps?

A new IR Memory SSA but does reasoning over that at function level?

Have a look at openMP IR.

Need the CG to make sure the openMP function encapsulates all the computation. 

Create a ticket with COSE IT Helpdesk - Ask for a linux account for access to GPG nodes.
Glasgow Parallelism Group

Nikela Server also