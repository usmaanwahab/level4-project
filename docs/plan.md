This is just a general idea so far.

Using a DAG to model memory dependencies by analysing LLVM IR load and store operations and finding which sections are accessing the same memory.

Building a pass in LLVM plugin to analyse load and stores then passing it to another pass that can highlight potential memory dependency issues.


- [x] Install prerequisites and dependencies for LLVM, OpenMP, and C/C++
- [ ] Create a basic plugin to count load and store instructions
- [ ] Extend this plugin with a DAG
- [ ] Extend this plugin to identify accesses and loads that are the same across methods.
	- I think initially check if the structure is shared or lastprivate somehow by checking what is passed to the openMP calls?
- [ ] Research static analysis
	- We now need some way to confidently say this is a memory dependency issue or not
