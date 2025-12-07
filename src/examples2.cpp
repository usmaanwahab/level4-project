#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <algorithm>

#include "arcana/noelle/core/NoellePass.hpp"
#include "svf/MemoryModel/PointerAnalysis.h"
#include "Graphs/SVFG.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include "Util/Options.h"
#include "WPA/Andersen.h"

using namespace llvm;
using namespace std;
using namespace SVF;
using namespace arcana::noelle;

namespace
{

  class MyDependenceAnalysis : public DependenceAnalysis
  {
  public:
    MyDependenceAnalysis()
        : DependenceAnalysis("Example of data dependence analysis"), c{0} {}

    bool canThereBeAMemoryDataDependence(Instruction *fromInst,
                                         Instruction *toInst,
                                         Function &f) override
    {
      errs() << this->prefix << "canThereBeAMemoryDataDependence: Function "
             << f.getName() << "\n";
      errs() << this->prefix << "canThereBeAMemoryDataDependence:   From "
             << *fromInst << "\n";
      errs() << this->prefix << "canThereBeAMemoryDataDependence:   To "
             << *toInst << "\n";
      errs() << this->prefix << "canThereBeAMemoryDataDependence:\n";

      c++;

      return true;
    }

    bool canThereBeAMemoryDataDependence(Instruction *fromInst,
                                         Instruction *toInst,
                                         LoopStructure &loop) override
    {
      auto entryInst = loop.getEntryInstruction();
      auto f = fromInst->getFunction();
      errs() << this->prefix
             << "canThereBeAMemoryDataDependence: Loop at nesting level "
             << loop.getNestingLevel() << ": " << *entryInst << "\n";
      errs() << this->prefix << "canThereBeAMemoryDataDependence:   In function "
             << f->getName() << "\n";
      errs() << this->prefix
             << "canThereBeAMemoryDataDependence:   Dependence from " << *fromInst
             << "\n";
      errs() << this->prefix << "canThereBeAMemoryDataDependence:   to "
             << *toInst << "\n";
      errs() << this->prefix << "canThereBeAMemoryDataDependence:\n";

      c++;

      return true;
    }

    ~MyDependenceAnalysis()
    {
      errs() << "The API has been invoked " << this->c << " times\n";
    }

  private:
    std::string prefix;
    uint64_t c;
  };

  struct CAT : public ModulePass
  {
    static char ID;

    CAT() : ModulePass(ID) {}

    bool doInitialization(Module &M) override { return false; }

    bool runOnModule(Module &M) override
    {

      auto &noelle = getAnalysis<NoellePass>().getNoelle();
      auto dfe = noelle.getDataFlowEngine();
      auto aliasAnalysisEngines = noelle.getAliasAnalysisEngines();
      
      AliasAnalysisEngine *aa = nullptr;
      for (auto e : aliasAnalysisEngines)
      {
        if (e->getName() == "ProgramAliasAnalysisEngine \"SVF\"")
        {
          aa = e;
          break;
        }
      }

      if (aa == nullptr)
      {
        outs() << "No engine selected, exiting...";
        return false;
      }

      auto *pta = static_cast<SVF::PointerAnalysis *>(aa->getRawPointer());

      auto computeGen = [](Instruction *i, DataFlowResult *df)
      {
        if (!isa<LoadInst>(i))
        {
          return;
        }
        auto &gen = df->GEN(i);
        gen.insert(i);
      };

      auto computeKILL = [](Instruction *, DataFlowResult *)
      { return; };

      auto computeOUT = [](Instruction *i, Instruction *successor, std::set<Value *> &OUT, DataFlowResult *df)
      {
        auto &inS = df->IN(successor);
        OUT.insert(inS.begin(), inS.end());
        return;
      };

      auto computeIN = [](Instruction *i, std::set<Value *> &IN, DataFlowResult *df)
      {
        auto &genI = df->GEN(i);
        auto &outI = df->OUT(i);
        IN.insert(outI.begin(), outI.end());
        IN.insert(genI.begin(), genI.end());
      };

      auto fm = noelle.getFunctionsManager();
      auto mainF = fm->getEntryFunction();

      auto customDfr = dfe.applyBackward(mainF, computeGen, computeKILL, computeIN, computeOUT);

      auto modset = LLVMModuleSet::getLLVMModuleSet();
      for (auto &F : M)
      {
        for (auto &If : instructions(F))
        {
          if (auto *Lf = dyn_cast<LoadInst>(&If))
          {
            for (auto &G : M)
            {
              for (auto &Ig : instructions(G))
              {
                if (auto *Lg = dyn_cast<LoadInst>(&Ig))
                {
                  if (Lf == Lg)
                  {
                    continue;
                  }

                  llvm::Value *ptr1 = Lf->getPointerOperand();
                  llvm::Value *ptr2 = Lg->getPointerOperand();
                  SVF::SVFValue *v1 = modset->getSVFValue(ptr1);
                  SVF::SVFValue *v2 = modset->getSVFValue(ptr2);
                  AliasAnalysis::alias(ptr1, ptr2);
                  auto result = pta->alias(v1, v2);
                  if (result == SVF::AliasResult::MayAlias)
                  {
                    errs() << "Instruction: " << Lf << " may alias " << Lg;
                  }
                  if (result == SVF::AliasResult::MustAlias)
                  {
                    errs() << "Instruction: " << Lf << " must alias " << Lg;
                  }
                }
              }
            }
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
} // namespace

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
static RegisterStandardPasses
    _RegPass2(PassManagerBuilder::EP_EnabledOnOptLevel0,
              [](const PassManagerBuilder &, legacy::PassManagerBase &PM)
              {
                if (!_PassMaker)
                {
                  PM.add(_PassMaker = new CAT());
                }
              }); // ** for -O0
