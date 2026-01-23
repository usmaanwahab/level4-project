Meeting 10 - 15/01/2026 @ 12:30
Attendees: Usmaan, Nikela

## Progress

Currently using the Program Dependence Graph (PDG) at instruction-level granularity to identify MUST-alias memory dependencies that are Read-after-Write (RAW).
These from what I gather are the only real actionable dependencies, and anything else seems less importable or requires more care and further analysis.

Observed that instruction-level analysis may be too fine-grained for practical optimization decisions.

Plan is to extend the LLVM pass to operate at the loop level, aggregating dependencies per loop rather than per instruction, which should provide more actionable information for memory locality and potential parallelization.

There also seems to be some functionality in Noelle to detect "hot" loops and such which may prove useful informaiton to provide to OpenMP.

I also am concenred about how focusing only on loop level maye be restricive such that we can't properly analyse tasks.

 
## Questions

Is looking at loop-level dependencies a better decision for memory locality and OpenMP parallelization?

For instruction-level dependencies, the only reasonable summary for the OpenMP runtime might be to mark each instruction with dependencies as a HashMap where the key is the instruction and the entry is a vector of instructions that the key is dependent on. Is this representation sufficient or too fine-grained?

How should we evaluate the quality and effectiveness of the dependency information provided to the OpenMP runtime? I'm not sure as it seem because there is no runtime implementation, it's more qualitative and also kind of speculative?
Would I need to provide maybe a brief high level design of how this informaiton can be used?

Are there cases where loop-carried dependencies might be misrepresented or missed using the current PDG-based approach?

Should we consider aggregating dependencies per memory location (rather than per instruction) to better reflect potential conflicts for OpenMP tasks?
