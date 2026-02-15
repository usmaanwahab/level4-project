#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "arcana/noelle/core/NoellePass.hpp"

using namespace arcana::noelle;

enum class AccessType
{
  Read,
  Write,
};

struct MemoryAccessNode
{
  Value *baseAddrPtr = nullptr;
  AccessType accessType = AccessType::Read;
  bool hasStride = false;
  int stride = 0;
  std::unordered_set<MemoryAccessNode *> dependsOn;

  MemoryAccessNode() = default;

  MemoryAccessNode(Value *ptr, AccessType type)
      : baseAddrPtr(ptr), accessType(type) {}
};

struct TaskInfo
{
  int id = -1;
  llvm::Function *outlinedFunction = nullptr;
  MemoryAccessNode *entryNode = nullptr;
  std::vector<MemoryAccessNode *> nodes;

  TaskInfo()
  {
    static int globalTaskId = 0;
    id = globalTaskId++;
  }

  TaskInfo(llvm::Function *func)
      : outlinedFunction(func)
  {
    static int globalTaskId = 0;
    id = globalTaskId++;
  }
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
      // Basic noelle setup
      auto &noelle = getAnalysis<NoellePass>().getNoelle();
      auto PDG = noelle.getProgramDependenceGraph();
      auto fm = noelle.getFunctionsManager();

      for (auto f : fm->getFunctions())
      {
        // only iterate over omp outlined functions
        if (f->getName().find(".omp_outlined") == std::string::npos)
          continue;

        auto FDG = PDG->createFunctionSubgraph(*f);

        // initalise task
        TaskInfo task;
        task.outlinedFunction = f;

        std::unordered_map<Value *, MemoryAccessNode *> nodeMap;

        for (auto &inst : instructions(f))
        {
          // only consider memory instructions
          if (!isa<LoadInst>(&inst) && !isa<StoreInst>(&inst))
            continue;

          bool isRead = isa<LoadInst>(&inst);
          Value *ptr = nullptr;

          if (auto load = dyn_cast<LoadInst>(&inst))
            ptr = load->getPointerOperand();
          else if (auto store = dyn_cast<StoreInst>(&inst))
            ptr = store->getPointerOperand();

          // create the MAN
          auto node = new MemoryAccessNode(ptr, isRead ? AccessType::Read : AccessType::Write);
          nodeMap[&inst] = node;
          task.nodes.push_back(node);

          if (!task.entryNode)
            task.entryNode = node;

          // collect incoming RAW-MUST memory dependencies
          std::vector<Value *> deps;

          auto iterF = [&deps](Value *src, DGEdge<Value, Value> *dep) -> bool
          {
            if (isa<ControlDependence<Value, Value>>(dep))
              return false;

            if (!dep->isLoopCarriedDependence())
              return false;

            auto dataDep = cast<DataDependence<Value, Value>>(dep);
            if (!dataDep->isRAWDependence())
              return false;

            if (!isa<MemoryDependence<Value, Value>>(dataDep))
              return false;

            auto memDep = cast<MemoryDependence<Value, Value>>(dataDep);
            if (!isa<MustMemoryDependence<Value, Value>>(memDep))
              return false;

            deps.push_back(src);
            return false;
          };

          FDG->iterateOverDependencesTo(&inst, true, true, true, iterF);

          // link dependencies to MAN
          for (auto *depInst : deps)
          {
            auto it = nodeMap.find(depInst);
            if (it != nodeMap.end())
            {
              node->dependsOn.insert(it->second);
            }
          }
        }

        // print TaskInfo
        errs() << "Task Info for function: " << f->getName() << "\n";
        errs() << "Memory nodes: " << task.nodes.size() << "\n";

        for (auto node : task.nodes)
        {
          errs() << "Node: " << *(node->baseAddrPtr) << " | read=" << (node->accessType == AccessType::Read ? "Read" : "Write") << "\n";

          if (!node->dependsOn.empty())
          {
            errs() << "  Depends on:\n";
            for (auto dep : node->dependsOn)
            {
              errs() << "    " << *(dep->baseAddrPtr) << "\n";
            }
          }
        }

        errs() << "\n";

        for (auto &inst : instructions(f))
        {
          // only consider memory instructions
          if (!isa<LoadInst>(&inst) && !isa<StoreInst>(&inst))
            continue;

          Value *ptr = nullptr;
          AccessType type;

          if (auto load = dyn_cast<LoadInst>(&inst))
          {
            ptr = load->getPointerOperand();
            type = AccessType::Read;
          }
          else if (auto store = dyn_cast<StoreInst>(&inst))
          {
            ptr = store->getPointerOperand();
            type = AccessType::Write;
          }

          // create or get MAN for this instruction
          MemoryAccessNode *node;
          auto it = nodeMap.find(&inst);
          if (it != nodeMap.end())
          {
            node = it->second;
          }
          else
          {
            node = new MemoryAccessNode(ptr, type);
            nodeMap[&inst] = node;
            task.nodes.push_back(node);

            if (!task.entryNode)
              task.entryNode = node;
          }

          // collect outgoing loop-carried RAW-MUST memory dependencies
          std::vector<Value *> deps;

          auto iterF = [&deps](Value *dst, DGEdge<Value, Value> *dep) -> bool
          {
            if (isa<ControlDependence<Value, Value>>(dep))
              return false;

            if (!dep->isLoopCarriedDependence())
              return false;

            auto dataDep = cast<DataDependence<Value, Value>>(dep);
            if (!dataDep->isRAWDependence())
              return false;

            if (!isa<MemoryDependence<Value, Value>>(dataDep))
              return false;

            auto memDep = cast<MemoryDependence<Value, Value>>(dataDep);
            if (!isa<MustMemoryDependence<Value, Value>>(memDep))
              return false;

            deps.push_back(dst);
            return false;
          };

          FDG->iterateOverDependencesFrom(&inst, true, true, true, iterF);

          // link nodes to MAN
          for (auto *depInst : deps)
          {
            auto depIt = nodeMap.find(depInst);
            if (depIt != nodeMap.end())
            {
              depIt->second->dependsOn.insert(node);
            }
          }
        }

        return false;
      }
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