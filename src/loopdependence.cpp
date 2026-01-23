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

        bool doInitialization(Module &m) override
        {
            return false;
        }

        bool runOnModule(Module &m) override
        {
            auto &noelle = getAnalysis<NoellePass>().getNoelle();
            // auto PDG = noelle.getProgramDependenceGraph();
            auto LNG = noelle.getLoopNestingGraphForProgram();
            auto loops = LNG->getLoopNodes();
            errs() << "=== Loop summary ===\n";

            for (auto loopNode : loops)
            {
                auto L = loopNode->getLoop();
                auto header = L->getHeader();
                Function *F = header->getParent();

                errs() << "Loop in function: " << F->getName() << "\n";
                errs() << "  Header BB: " << header->getName() << "\n";

                errs() << "  # BasicBlocks: "
                       << L->getBasicBlocks().size() << "\n";

                errs() << "\n";
            }
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