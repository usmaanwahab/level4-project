#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <algorithm>

#include "arcana/noelle/core/NoellePass.hpp"

using namespace arcana::noelle;

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
    auto &noelle = getAnalysis<Noelle>();

    auto PDG = noelle.getProgramDependenceGraph();
    auto fm = noelle.getFunctionsManager();

    for (auto f: fm->getFunctions()) {

    }
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override
  {
    AU.addRequired<NoellePass>();
  }
};

char CAT::ID = 0;
static RegisterPass<CAT> X("CAT", "Simple user of the Noelle framework");

static CAT *_PassMaker = NULL;
static RegisterStandardPasses _RegPass1(PassManagerBuilder::EP_OptimizerLast,
                                        [](const PassManagerBuilder &,
                                           legacy::PassManagerBase &PM)
                                        {
                                          if (!_PassMaker)
                                          {
                                            PM.add(_PassMaker = new CAT());
                                          }
                                        });

static RegisterStandardPasses _RegPass2(
    PassManagerBuilder::EP_EnabledOnOptLevel0,
    [](const PassManagerBuilder &, legacy::PassManagerBase &PM)
    {
      if (!_PassMaker)
      {
        PM.add(_PassMaker = new CAT());
      }
    });
