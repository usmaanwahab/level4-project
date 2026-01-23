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
            auto found = false;

            auto iterF = [&found](Value *src, DGEdge<Value, Value> *dep) -> bool
            {
                if (isa<ControlDependence<Value, Value>>(dep))
                {
                    return false;
                }
                else
                {
                    auto dataDep = cast<DataDependence<Value, Value>>(dep);
                    if (dataDep->isRAWDependence())
                    {
                        if (isa<MemoryDependence<Value, Value>>(dataDep))
                        {
                            auto memDep = cast<MemoryDependence<Value, Value>>(dataDep);
                            if (isa<MustMemoryDependence<Value, Value>>(memDep))
                            {
                                found = true;
                                errs() << "    " << *src << " DATA " << " RAW " << " MEMORY " << " MUST \n";
                            }
                        }
                    }
                }
                return false;
            };

            for (auto f : fm->getFunctions())
            {
                auto FDG = PDG->createFunctionSubgraph(*f);
                for (auto &inst : instructions(f))
                {
                    FDG->iterateOverDependencesTo(&inst, true, true, true, iterF);
                    if (found)
                    {
                        errs() << "^^Instruction \"" << inst << "\" depends on ^^^\n";
                        found = false;
                    }
                }

                for (auto &inst : instructions(f))
                {
                    FDG->iterateOverDependencesFrom(&inst, true, true, true, iterF);
                    if (found)
                    {
                        errs() << "^^Instruction \"" << inst << "\" outgoing dependencies^^^\n";
                        found = false;
                    }
                }
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