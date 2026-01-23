#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "arcana/noelle/core/NoellePass.hpp"

using namespace arcana::noelle;

namespace
{
  struct CAT : public ModulePass
  {
    static char ID;
    CAT() : ModulePass(ID) {}

    bool doInitialization(Module &M) override
    {
      return false;
    }

    bool runOnModule(Module &M) override
    {
      auto &noelle = getAnalysis<NoellePass>().getNoelle();
      auto PDG = noelle.getProgramDependenceGraph();
      auto fm = noelle.getFunctionsManager();
      auto main = fm->getEntryFunction();

      errs() << "=================================\n";
      errs() << "PROGRAM DEPENDENCE GRAPH (PDG)\n";
      errs() << "=================================\n";
      PDG->print(errs());

      // errs() << "=================================\n";
      // errs() << "FUNCTION DEPENDENCE GRAPH (PDG)\n";
      // errs() << "=================================\n";
      // PDG->createFunctionSubgraph(*main)->print(errs());

      // std::unordered_map<Value *, std::vector<DGEdge<Value, Value> *>> depsByObject;
      // auto iterF = [&](Value *src, DGEdge<Value, Value> *dep) -> bool
      // {
      //   if (!isa<Instruction>(src)) {
      //     return false;
      //   }
      //   auto srcInstruction = dyn_cast<Instruction>(src);
      //   srcInstruction->print(errs());
      //   if (isa<LoadInst>(srcInstruction)) {
      //     auto *L = dyn_cast<LoadInst>(srcInstruction);
      //     errs() << "--" << L->getPointerOperand();
      //   } else if (isa<StoreInst>(srcInstruction)) {
      //     auto *S = dyn_cast<StoreInst>(srcInstruction);
      //     errs() <<  "--" << S->getPointerOperand();
      //   }
      //   errs() << "\n";
      //   return false;
      // };
      // for (auto f : fm->getFunctions())
      // {
      //   auto FDG = PDG->createFunctionSubgraph(*f);

      //   for (auto &inst : instructions(f))
      //   {
      //     FDG->iterateOverDependencesFrom(&inst, true, false, true, iterF);
      //   }
      // }

      return false;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override
    {
      AU.addRequired<NoellePass>();
    }
  };
}

// Next there is code to register your pass to "opt"
char CAT::ID = 0;
static RegisterPass<CAT> X("CAT", "Simple user of the Noelle framework");
// Next there is code to register your pass to "clang"
static CAT *_PassMaker = NULL;
static RegisterStandardPasses _RegPass1(PassManagerBuilder::EP_OptimizerLast,
                                        [](const PassManagerBuilder &,
                                           legacy::PassManagerBase &PM)
                                        {
                                          if (!_PassMaker)
                                          {
                                            PM.add(_PassMaker = new CAT());
                                          }
                                        }); // ** for -Ox
static RegisterStandardPasses _RegPass2(
    PassManagerBuilder::EP_EnabledOnOptLevel0,
    [](const PassManagerBuilder &, legacy::PassManagerBase &PM)
    {
      if (!_PassMaker)
      {
        PM.add(_PassMaker = new CAT());
      }
    }); // ** for -O0
