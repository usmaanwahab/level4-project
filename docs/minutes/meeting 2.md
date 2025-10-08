
**Date & Time:** 1-10-2025 @ 11:30  
Attendees: Usmaan & Nikela

---

## Progress
- Completed reading "CesASMe and Staticdeps: static detection of memory-carried
dependencies for code analyzers"
- Completed reading "NOELLE Offers Empowering LLVM Extensions"
- Completed reading "Bridging Control-Centric and Data-Centric Optimization"
---

## Questions
- Difficulty interpreting Figure 3 in NOELLE Paper
- How long is too long to spend reading? I feel like I should spend at least the first 2-3 weeks reading? but I am not sure?
- Should I build my tool on-top of existing tools like NOELLE and CeASMe?
- Should I build OpenMP locally, this will allow me to enable tracing? I believe? Currently I just have a binary from yay 
- Should I speak to other Lecturers? i.e. I was thinking of speaking to Simon Fowler or Jeremy Singer?
- Is skimming papers better? I don't feel like I pick up on much information if I don't read it properly?
- How many papers should I aim to read per week?
- From reading I've found that more emphasis is put on Read-After-Write dependencies.
---

## Answers / Decisions
- The bars are stacked in reverse. 
- Continue reading papers in parallel with development and it may be wise to print and annotate/highlight papers.
- Building OpenMP locally is valuable but slightly tedious.
- Yes, RAW dependencies are the hardest of the three as the other two can be at least partially mitigated.
- Speak with Wim who knows more about the Compilers and LLVM

---

## Action Items
- [x] Download LLVM 14
- [x] Download LLVM 20
- [x] Ensure it can coexist with latest LLVM
- [x] Setup NOELLE on LLVM 14
- [ ] Test basic NOELLE features and record information
- [ ] Setup CeASMe
- [ ] Test basic CeASMe features and record information
- [ ] Test basic LLVM features and record information
- [ ] Compare and contrast the three methods
- [x] Give Nikela a link of the GitHub
- [ ] Read into "reuse" distance
- [x] Engage with NOELLE video content

---

## Quick Notes (during meeting)

Compiler papers are hard to skim.

Start with LLVM.

Building on top of NOELLE is wise.

Read papers in parallel

Printing and annotating papers.

NOELLE has videos - Search them

Try to setup NOELLE and CeASMe.
Compare them to what LLVM analyser gives you.

Speak to Vim, Jeremey, Simon.

Provide Nikela with GitHub link.

Upload meeting minutes to MOODLE!!!


Reuse distance
I access a memory location now and I access again after after $n$ instructions. - Temporal

Accessing this location now and neighbouring location a little bit later. - Spacial

Get some idea of reuse distance from static analysis and such.
https://github.com/usmaanwahab/level4-project
