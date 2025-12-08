
**Date & Time:** 12-11-2025 @ 11:30  
Attendees: Usmaan & Nikela

---

## Progress
- Putting aside the idea to develop the analysis pass on LLVM20, I don't see the benefit or utility.
- Significant issues building SVF for alias analysis now resolved.
- Working towards detecting if two threads (i.e. openMP outline functions) access the same memory. Then based on pattern could recommend different solutions. For example, two threads only read a variable set as private, is there then a need for this unnecessary (possible expensive) copy? Threads read and write to a shared variable, is privitsation, accumulation or something else possible, to avoid false sharing?
- Some minor background reading in Data Flow Analysis, just basic definitions really 
- Possible to use noelles profiler to better understand a program and provide feedback? (unsure on this one)
---

## Questions
- Is it fine to abandon llvm20? Maybe revisit at a later stage, but from reading and general docs it just seems like it will be significantly slower to develop in pure LLVM20
- Can I only access the fisherman server when connected to the VPN?---

## Answers / Decisions
- 

---

## Action Items
- [ ] Investigate llvm20
- [ ]

---

## Quick Notes (during meeting)

Not wise to outright disregard llvm20, it could provide significant improvements.

We don't want to touch pragams or directives to avoid changing program semantics

