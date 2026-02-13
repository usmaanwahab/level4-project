#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "arcana/noelle/core/NoellePass.hpp"

using namespace arcana::noelle;

struct MemoryAccessNode
{
  Value *baseAddrPtr;
  bool isRead;
  bool hasStride;
  int stride;
  std::unordered_set<MemoryAccessNode> dependsOn;
};

struct TaskInfo
{
  int id;
  llvm::Function *outlinedFunction;
  MemoryAccessNode entryNode;
  std::vector<MemoryAccessNode *> nodes;
};

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

      for (auto f : fm->getFunctions())
      {
        if (f->getName().find(".omp_outlined") == std::string::npos)
          continue;

        auto FDG = PDG->createFunctionSubgraph(*f);
        for (auto &inst : instructions(f))
        {
          std::vector<Value *> deps;

          auto iterF = [&deps](Value *src, DGEdge<Value, Value> *dep) -> bool
          {
            if (isa<ControlDependence<Value, Value>>(dep))
              return false;

            auto dataDep = cast<DataDependence<Value, Value>>(dep);
            if (dataDep->isRAWDependence())
            {
              if (isa<MemoryDependence<Value, Value>>(dataDep))
              {
                auto memDep = cast<MemoryDependence<Value, Value>>(dataDep);
                if (isa<MustMemoryDependence<Value, Value>>(memDep))
                {
                  deps.push_back(src);
                }
              }
            }
            return false;
          };

          FDG->iterateOverDependencesTo(&inst, true, true, true, iterF);

          if (!deps.empty())
          {
            for (auto *d : deps)
            {
              errs() << "Instruction \"" << inst << "\" incoming dependencies:\n";
              for (auto *d : deps)
              {
                errs() << "    " << *d << " DATA RAW MEMORY MUST\n";
              }
              errs() << "\n";
            }
          }
        }

        for (auto &inst : instructions(f))
        {
          std::vector<Value *> deps;
          // TODO: inst.setMetadata
          auto iterF = [&deps](Value *dst, DGEdge<Value, Value> *dep) -> bool
          {
            if (isa<ControlDependence<Value, Value>>(dep))
            {
              return false;
            }
            if (!dep->isLoopCarriedDependence())
            {
              return false;
            }
            auto dataDep = cast<DataDependence<Value, Value>>(dep);
            if (dataDep->isRAWDependence())
            {
              if (isa<MemoryDependence<Value, Value>>(dataDep))
              {
                auto memDep = cast<MemoryDependence<Value, Value>>(dataDep);
                if (isa<MustMemoryDependence<Value, Value>>(memDep))
                {
                  deps.push_back(dst);
                }
              }
            }
            return false;
          };

          FDG->iterateOverDependencesFrom(&inst, true, true, true, iterF);

          if (!deps.empty())
          {
            errs() << "Instruction \"" << inst << "\" outgoing dependencies:\n";
            for (auto *d : deps)
            {
              errs() << "    " << *d << " DATA RAW MEMORY MUST\n";
            }
            errs() << "\n";
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