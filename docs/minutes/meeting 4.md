**Date & Time:** 14-10-2025 @ 11:30  
Attendees: Usmaan & Nikela

---
## Progress
- More reading 
- Running Noelle examples
- Investigating how to determine if two variables may overlap
- Investigating how to determine if two variables are near enough to each other
- Investigating what Noelle provides that I can exploit the graphs it provides

---
## Questions
- How would I go about looking into where OpenMP would partition arrays and such?
- I am thinking more on the NUMA level rather than single chip machines, is that right or should I consider both?
- 

---
## Answers / Decisions
- That may not be necessary but Nikela will get back to me
- NUMA is irrelevant 

---
## Action Items
- [ ] Enable tracing and debug in OpenMP
- [ ] Continue learning about memory dependencies
- [ ] Continue exploring NOELLE

---
## Quick Notes (during meeting)

OpenMP is mapped into LLVM IR

Set the debug levels to Maximum 
Not worth statically building.

NUMA and topology are irrelevant for me

Enable OpenMP Tracing

XWLOCK for memory topology

KT_TRACE debug level.

Clear path on how to do memory dependecy analysis
how to do it the ir or runtime should be mostly compelte
have an understanding of openMP runtime