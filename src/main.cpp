#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>

using namespace llvm;

struct ByeModule : PassInfoMixin<ByeModule> {
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM) {
    int store_count = 0;
    int load_count = 0;

    for (Function &F : M) {
      errs() << "Hello: " << F.getName() << "\n";
      for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
          if (isa<LoadInst>(I)) {
            load_count++;
          }
          if (isa<StoreInst>(I)) {
            store_count++;
          }
        }
      }
    }

    errs() << "stores: " << store_count << "\nloades: " << load_count;
    return PreservedAnalyses::all();
  }
};

llvm::PassPluginLibraryInfo getByePluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "HelloWorld", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "hello-world") {
                    MPM.addPass(ByeModule());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getByePluginInfo();
}
