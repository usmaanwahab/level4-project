#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/Support/raw_ostream.h"
#include <set>
#include <map>
#include "llvm/Analysis/ValueTracking.h"

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
      auto functions = noelle.getFunctionsManager()->getFunctions();
      std::unordered_map<Function *, std::unordered_set<DGNode<llvm::Value> *>> fsgExternalDeps;

      for (Function *f : functions)
      {
        if (f->getName().find(".omp_outlined") == std::string::npos)
          continue;

        auto *FSG = PDG->createFunctionSubgraph(*f);

        std::unordered_set<DGNode<llvm::Value> *> fsgNodes;
        for (auto *n : FSG->getNodes())
        {
          fsgNodes.insert(n);
        }

        std::unordered_set<DGNode<llvm::Value> *> externalDeps;

        for (auto *n : fsgNodes)
        {
          for (auto *edge : n->getAllEdges())
          {

            auto *src = edge->getSrcNode();
            if (!fsgNodes.count(src))
            {
              externalDeps.insert(src);
            }
            // errs() << "External dep for " << f->getName() << ": ";
            // src->getT()->print(errs());
            // errs() << "\n";
          }
        }
        fsgExternalDeps[f] = std::move(externalDeps);
      }
      for (auto &pair : fsgExternalDeps)
      {
        errs() << "External deps for " << pair.first->getName() << ":\n";
        for (auto *dep : pair.second)
        {
          dep->getT()->print(errs());
          errs() << "\n";
        }
      }
      for (auto it1 = fsgExternalDeps.begin(); it1 != fsgExternalDeps.end(); ++it1)
      {
        for (auto it2 = std::next(it1); it2 != fsgExternalDeps.end(); ++it2)
        {
          for (auto *depNode1 : it1->second)
          {
            Value *dep1 = depNode1->getT();
            // Value *canon1 = dep1->stripPointerCasts();

            for (auto *depNode2 : it2->second)
            {
              Value *dep2 = depNode2->getT();
              // Value *canon2 = dep2->stripPointerCasts();
              Value *canon1 = llvm::getUnderlyingObject(dep1);
              Value *canon2 = llvm::getUnderlyingObject(dep2);
              if (canon1 == canon2)
              {
                errs() << "Shared external dep between "
                       << it1->first->getName() << " and "
                       << it2->first->getName() << ": ";
                dep1->getType()->print(errs());
                errs() << " (";
                canon1->print(errs());
                errs() << ")\n";
              }
            }
          }
        }
      }

      // for (Function *f : functions)
      // {
      //   if (f->getName().find(".omp_outlined") == std::string::npos)
      //   {
      //     continue;
      //   }

      //   auto FSG = PDG->createFunctionSubgraph(*f);

      //   std::unordered_set<DGNode<llvm::Value> *> fsgNodes;
      //   for (auto *n : FSG->getNodes())
      //   {
      //     fsgNodes.insert(n);
      //   }
      //   for (auto *pdgNode : PDG->getNodes())
      //   {

      //     if (fsgNodes.count(pdgNode))
      //     {
      //       continue;
      //     }

      //     bool hasCrossDep = false;
      //     for (auto *edge : pdgNode->getOutgoingEdges())
      //     {
      //       auto dst = edge->getDstNode();
      //       if (fsgNodes.count(dst))
      //       {
      //         hasCrossDep = true;
      //         break;
      //       }
      //     }

      //     if (hasCrossDep)
      //     {
      //       errs() << "External node with dependency to FSG:\n";
      //       pdgNode->getT()->print(errs());
      //       errs() << "\n";
      //     }
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
