#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "arcana/noelle/core/NoellePass.hpp"

using namespace arcana::noelle;

namespace {
struct CAT : public ModulePass {
  static char ID;
  CAT() : ModulePass(ID) {}

  bool doInitialization(Module &M) override {
    return false;
  }

  bool runOnModule(Module &M) override {
    auto &noelle = getAnalysis<NoellePass>().getNoelle();
    auto PDG = noelle.getProgramDependenceGraph();
    auto fm = noelle.getFunctionsManager();

    errs() << "=====================================\n";
    errs() << "PROGRAM DEPENDENCE ANALYSIS\n";
    errs() << "=====================================\n\n";

    // === LOOP SUMMARY ===
    auto LNG = noelle.getLoopNestingGraphForProgram();
    auto loops = LNG->getLoopNodes();

    errs() << "=== Loop summary ===\n";
    for (auto loopNode : loops) {
      auto L = loopNode->getLoop();
      auto header = L->getHeader();
      Function *F = header->getParent();

      errs() << "Loop in function: " << F->getName() << "\n";
      errs() << "  Header BB: " << header->getName() << "\n";
      errs() << "  # BasicBlocks: " << L->getBasicBlocks().size() << "\n";
      errs() << "\n";
    }

    errs() << "\n=====================================\n";
    errs() << "DEPENDENCE DETAILS PER FUNCTION\n";
    errs() << "=====================================\n";

    for (auto f : fm->getFunctions()) {
      if (f->isDeclaration()) continue;

      errs() << "\n=====================================\n";
      errs() << "Function: " << f->getName() << "\n";
      errs() << "=====================================\n";

      auto FDG = PDG->createFunctionSubgraph(*f);

      for (auto &inst : instructions(f)) {
        bool foundIncoming = false;
        bool foundOutgoing = false;

        errs() << "\n-------------------------------------\n";
        errs() << "Instruction: " << inst << "\n";

        // === Incoming dependencies ===
        auto incomingIter = [&](Value *src, DGEdge<Value, Value> *dep) -> bool {
          if (isa<ControlDependence<Value, Value>>(dep)) {
            errs() << "  <- CONTROL from: " << *src << "\n";
            foundIncoming = true;
            return false;
          }

          auto dataDep = cast<DataDependence<Value, Value>>(dep);

          errs() << "  <- DATA ";
          if (dataDep->isRAWDependence()) errs() << "RAW ";
          else if (dataDep->isWARDependence()) errs() << "WAR ";
          else if (dataDep->isWAWDependence()) errs() << "WAW ";

          if (isa<MemoryDependence<Value, Value>>(dataDep)) {
            auto memDep = cast<MemoryDependence<Value, Value>>(dataDep);
            errs() << "MEMORY ";

            if (isa<MustMemoryDependence<Value, Value>>(memDep)) {
              errs() << "MUST ";
            } else {
              errs() << "MAY ";
            }
          } else {
            errs() << "REGISTER ";
          }

          errs() << "from: " << *src << "\n";
          foundIncoming = true;
          return false;
        };

        FDG->iterateOverDependencesTo(&inst, true, true, true, incomingIter);

        if (!foundIncoming) {
          errs() << "  <- No incoming dependencies\n";
        }

        // === Outgoing dependencies ===
        auto outgoingIter = [&](Value *dst, DGEdge<Value, Value> *dep) -> bool {
          if (isa<ControlDependence<Value, Value>>(dep)) {
            errs() << "  -> CONTROL to: " << *dst << "\n";
            foundOutgoing = true;
            return false;
          }

          auto dataDep = cast<DataDependence<Value, Value>>(dep);

          errs() << "  -> DATA ";
          if (dataDep->isRAWDependence()) errs() << "RAW ";
          else if (dataDep->isWARDependence()) errs() << "WAR ";
          else if (dataDep->isWAWDependence()) errs() << "WAW ";

          if (isa<MemoryDependence<Value, Value>>(dataDep)) {
            auto memDep = cast<MemoryDependence<Value, Value>>(dataDep);
            errs() << "MEMORY ";

            if (isa<MustMemoryDependence<Value, Value>>(memDep)) {
              errs() << "MUST ";
            } else {
              errs() << "MAY ";
            }
          } else {
            errs() << "REGISTER ";
          }

          errs() << "to: " << *dst << "\n";
          foundOutgoing = true;
          return false;
        };

        FDG->iterateOverDependencesFrom(&inst, true, true, true, outgoingIter);

        if (!foundOutgoing) {
          errs() << "  -> No outgoing dependencies\n";
        }
      }
    }

    errs() << "\n=====================================\n";
    errs() << "END OF DEPENDENCE REPORT\n";
    errs() << "=====================================\n";

    return false;
  }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
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
		legacy::PassManagerBase &PM) {
			if (!_PassMaker) {
				PM.add(_PassMaker = new CAT());
			}
		}); // ** for -Ox
		static RegisterStandardPasses _RegPass2(
			PassManagerBuilder::EP_EnabledOnOptLevel0,
			[](const PassManagerBuilder &, legacy::PassManagerBase &PM) {
				if (!_PassMaker) {
					PM.add(_PassMaker = new CAT());
				}
			}); // ** for -O0
			
