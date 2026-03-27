#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Analysis/ValueTracking.h"

#include "arcana/noelle/core/NoellePass.hpp"
#include <unordered_set>
#include <vector>
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Alignment.h"

#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"

using namespace arcana::noelle;

namespace
{
  enum class AccessType
  {
    Read,
    Write,
  };

  enum class AliasType
  {
    Must,
    May
  };

  struct MemoryAccessNode;

  struct MemoryDependenceEdge
  {
    MemoryAccessNode *src = nullptr;
    MemoryAccessNode *dst = nullptr;
    AliasType aliasType = AliasType::May;
  };

  struct MemoryAccessNode
  {
    Value *baseAddrPtr = nullptr;
    AccessType accessType = AccessType::Read;
    bool hasStride = false;
    int stride = 0;

    std::vector<MemoryDependenceEdge *> incoming;
    std::vector<MemoryDependenceEdge *> outgoing;

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
    std::vector<std::unique_ptr<MemoryDependenceEdge>> edges;

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

  static Constant *converToI32(LLVMContext &ctx, int32_t V)
  {
    return ConstantInt::get(Type::getInt32Ty(ctx), V, true);
  }

  static GlobalVariable *emitTaskGlobal(Module &M, const TaskInfo &Task)
  {
    LLVMContext &ctx = M.getContext();

    Type *I32Type = Type::getInt32Ty(ctx);
    Type *PtrType = PointerType::getUnqual(ctx);

    // struct EmbeddableNode
    // {
    //   int32_t nodeId;
    //   void   *basePtr;
    //   int32_t accessType;  // 0 = Read, 1 = Write
    //   int32_t hasStride;
    //   int32_t stride;
    // };
    llvm::StructType *EmbeddedNodeType = StructType::get(ctx, {I32Type, PtrType, I32Type, I32Type, I32Type});

    // struct EmbeddableEdge
    // {
    //   int32_t srcNodeId;
    //   int32_t dstNodeId;
    //   int32_t aliasType;   // 0 = Must, 1 = May
    // };
    llvm::StructType *EmbeddedEdgeType = StructType::get(ctx, {I32Type, I32Type, I32Type});

    // struct EmbeddableTask
    // {
    //   int32_t         taskId;
    //   void           *outlinedFunction;
    //   int32_t         numNodes;
    //   EmbeddableNode *nodes;
    //   int32_t         numEdges;
    //   EmbeddableEdge *edges;
    // };
    llvm::StructType *EmbeddedTaskType = StructType::get(ctx, {I32Type, PtrType, I32Type, PtrType, I32Type, PtrType});

    std::map<MemoryAccessNode *, int32_t> NodeIDs;
    for (size_t i = 0; i < Task.nodes.size(); ++i)
    {
      NodeIDs[Task.nodes[i]] = static_cast<int32_t>(i);
    }

    // Emit node array
    std::vector<Constant *> NodeConsts;
    NodeConsts.reserve(Task.nodes.size());

    for (size_t i = 0; i < Task.nodes.size(); ++i)
    {
      MemoryAccessNode *N = Task.nodes[i];

      Constant *BasePtr = ConstantPointerNull::get(cast<PointerType>(PtrType));
      if (N->baseAddrPtr)
      {
        if (llvm::GlobalValue *GV = dyn_cast<GlobalValue>(N->baseAddrPtr))
        {
          BasePtr = ConstantExpr::getPointerCast(GV, PtrType);
        }
      }

      Constant *Fields[] = {
          converToI32(ctx, static_cast<int32_t>(i)),
          BasePtr,
          converToI32(ctx, N->accessType == AccessType::Read ? 0 : 1),
          converToI32(ctx, N->hasStride ? 1 : 0),
          converToI32(ctx, N->stride)};

      NodeConsts.push_back(ConstantStruct::get(EmbeddedNodeType, Fields));
    }

    llvm::ArrayType *NodeArrType = ArrayType::get(EmbeddedNodeType, NodeConsts.size());
    llvm::Constant *NodeArrInit = ConstantArray::get(NodeArrType, NodeConsts);

    llvm::GlobalVariable *NodesGV = new GlobalVariable(M, NodeArrType, true, GlobalValue::PrivateLinkage, NodeArrInit, "__task_dependency_info" + std::to_string(Task.id) + "_nodes");
    NodesGV->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
    NodesGV->setAlignment(Align(8));
    NodesGV->setSection(".taskgraph");

    Constant *NodesPtr = ConstantPointerNull::get(cast<PointerType>(PtrType));
    if (!Task.nodes.empty())
    {
      Constant *Zero = converToI32(ctx, 0);
      Constant *Idxs[] = {Zero, Zero};
      Constant *FirstNode = ConstantExpr::getInBoundsGetElementPtr(NodeArrType, NodesGV, Idxs);
      NodesPtr = ConstantExpr::getPointerCast(FirstNode, PtrType);
    }

    std::vector<Constant *> EdgeConsts;
    EdgeConsts.reserve(Task.edges.size());

    for (const auto &E : Task.edges)
    {
      auto srcIt = NodeIDs.find(E->src);
      auto dstIt = NodeIDs.find(E->dst);
      if (srcIt == NodeIDs.end() || dstIt == NodeIDs.end())
      {
        continue;
      }

      Constant *Fields[] = {
          converToI32(ctx, srcIt->second),
          converToI32(ctx, dstIt->second),
          converToI32(ctx, E->aliasType == AliasType::Must ? 0 : 1)};

      EdgeConsts.push_back(ConstantStruct::get(EmbeddedEdgeType, Fields));
    }

    Constant *EdgesPtr = ConstantPointerNull::get(cast<PointerType>(PtrType));

    if (!EdgeConsts.empty())
    {
      llvm::ArrayType *EdgeArrType = ArrayType::get(EmbeddedEdgeType, EdgeConsts.size());
      llvm::Constant *EdgeArrInit = ConstantArray::get(EdgeArrType, EdgeConsts);

      llvm::GlobalVariable *EdgesGV = new GlobalVariable(M, EdgeArrType, true, GlobalValue::PrivateLinkage, EdgeArrInit, "__task_dependency_info" + std::to_string(Task.id) + "_edges");
      EdgesGV->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
      EdgesGV->setAlignment(Align(4));
      EdgesGV->setSection(".taskgraph");

      Constant *Zero = converToI32(ctx, 0);
      Constant *Idxs[] = {Zero, Zero};
      Constant *FirstEdge = ConstantExpr::getInBoundsGetElementPtr(EdgeArrType, EdgesGV, Idxs);
      EdgesPtr = ConstantExpr::getPointerCast(FirstEdge, PtrType);
    }

    Constant *FnPtr = ConstantPointerNull::get(cast<PointerType>(PtrType));
    if (Task.outlinedFunction)
    {
      FnPtr = ConstantExpr::getPointerCast(Task.outlinedFunction, PtrType);
    }

    Constant *TaskFields[] = {
        converToI32(ctx, Task.id),
        FnPtr,
        converToI32(ctx, static_cast<int32_t>(Task.nodes.size())),
        NodesPtr,
        converToI32(ctx, static_cast<int32_t>(Task.edges.size())),
        EdgesPtr};

    Constant *TaskInit = ConstantStruct::get(EmbeddedTaskType, TaskFields);

    llvm::GlobalVariable *TaskGV = new GlobalVariable(M, EmbeddedTaskType, true, GlobalValue::ExternalLinkage, TaskInit, "__task_dependency_info" + std::to_string(Task.id));
    TaskGV->setAlignment(Align(8));
    TaskGV->setSection(".taskgraph");

    return TaskGV;
  }

  static void injectTaskDescriptor(Module &M, Function *outlinedFn, GlobalVariable *TaskGV)
  {
    LLVMContext &ctx = M.getContext();
    Type *PtrType = PointerType::getUnqual(ctx);

    std::vector<CallInst *> removeCIs;
    std::vector<CallInst *> pushCIs;

    for (Function &F : M)
    {
      for (BasicBlock &BB : F)
      {
        for (Instruction &I : BB)
        {
          CallInst *CI = dyn_cast<CallInst>(&I);
          if (!CI)
          {
            continue;
          }

          Function *callee = CI->getCalledFunction();
          if (!callee || callee->getName() != "__kmpc_fork_call")
          {
            continue;
          }

          Value *fnArg = CI->getArgOperand(2)->stripPointerCasts();
          if (fnArg != outlinedFn)
          {
            continue;
          }

          std::vector<Value *> newArgs(CI->arg_begin(), CI->arg_end());
          newArgs.push_back(ConstantExpr::getPointerCast(TaskGV, PtrType));

          CallInst *newCI = CallInst::Create(CI->getFunctionType(), CI->getCalledOperand(), newArgs, "", CI);
          newCI->setCallingConv(CI->getCallingConv());
          newCI->setAttributes(CI->getAttributes());
          removeCIs.push_back(CI);
          pushCIs.push_back(newCI);
        }
      }
    }

    for (size_t i = 0; i < removeCIs.size(); i++)
    {
      removeCIs[i]->replaceAllUsesWith(pushCIs[i]);
      removeCIs[i]->eraseFromParent();
    }
  }

  void printDeps(const TaskInfo &task, Function *f)
  {
    errs() << "========================================\n";
    errs() << "Task ID: " << task.id << "\n";
    errs() << "Function: " << f->getName() << "\n";
    errs() << "Outlined function ptr: " << task.outlinedFunction << "\n";
    errs() << "Total Memory Nodes: " << task.nodes.size() << "\n";
    errs() << "Total Edges:        " << task.edges.size() << "\n";
    errs() << "========================================\n\n";

    std::unordered_map<const MemoryAccessNode *, size_t> nodeIndex;
    for (size_t i = 0; i < task.nodes.size(); ++i)
    {
      nodeIndex[task.nodes[i]] = i;
    }

    for (size_t i = 0; i < task.nodes.size(); ++i)
    {
      MemoryAccessNode *node = task.nodes[i];

      errs() << "Node #" << i;
      if (node == task.entryNode)
      {
        errs() << " (ENTRY)";
      }
      errs() << "\n";

      errs() << "  Instruction : ";
      node->baseAddrPtr->print(errs());
      errs() << "\n";

      errs() << "  Pointer     : " << node->baseAddrPtr << "\n";

      errs() << "  Access Type : "
             << (node->accessType == AccessType::Read ? "READ" : "WRITE")
             << "\n";

      errs() << "  Has Stride  : " << (node->hasStride ? "yes" : "no") << "\n";

      if (node->hasStride)
      {
        errs() << "  Stride      : " << node->stride << "\n";
      }

      errs() << "  Incoming Edges: " << node->incoming.size() << "\n";

      if (!node->incoming.empty())
      {
        errs() << "  Depends On:\n";
        for (MemoryDependenceEdge *e : node->incoming)
        {
          errs() << "    <- Node #" << nodeIndex[e->src]
                 << "  [" << (e->aliasType == AliasType::Must ? "MUST" : "MAY") << "]\n";
          errs() << "       Inst : ";
          e->src->baseAddrPtr->print(errs());
          errs() << "\n";
          errs() << "       Access Type : "
                 << (e->src->accessType == AccessType::Read ? "READ" : "WRITE")
                 << "\n";
        }
      }

      errs() << "\n";
    }

    errs() << "========================================\n\n";
  }

  bool isMustMemoryDep(DGEdge<Value, Value> *dep)
  {
    DataDependence<Value, Value> *dataDep = cast<DataDependence<Value, Value>>(dep);
    if (!isa<MemoryDependence<Value, Value>>(dataDep))
    {
      return false;
    }

    MemoryDependence<Value, Value> *memDep = cast<MemoryDependence<Value, Value>>(dataDep);
    if (!isa<MustMemoryDependence<Value, Value>>(memDep))
    {
      return false;
    }

    return true;
  }

  void topoSortDFS(MemoryAccessNode *node, std::unordered_set<MemoryAccessNode *> &visited, std::vector<MemoryAccessNode *> &sorted)
  {
    if (visited.count(node))
    {
      return;
    }
    visited.insert(node);

    for (MemoryDependenceEdge *e : node->incoming)
    {
      topoSortDFS(e->src, visited, sorted);
    }

    sorted.push_back(node);
  }

  std::vector<MemoryAccessNode *> topologicalSort(const std::vector<MemoryAccessNode *> &nodes)
  {
    std::unordered_set<MemoryAccessNode *> visited;
    std::vector<MemoryAccessNode *> sorted;

    for (MemoryAccessNode *node : nodes)
    {
      topoSortDFS(node, visited, sorted);
    }

    return sorted;
  }

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
      auto FM = noelle.getFunctionsManager();

      int totalNodeCount = 0;

      for (auto f : FM->getFunctions())
      {
        if (f->getName().find(".omp_outlined") == std::string::npos && f->getName().find(".omp_task_entry") == std::string::npos)
        {
          continue;
        }

        auto FDG = PDG->createFunctionSubgraph(*f);
        auto &SE = getAnalysis<ScalarEvolutionWrapperPass>(*f).getSE();

        TaskInfo task;
        task.outlinedFunction = f;

        std::map<Value *, MemoryAccessNode *> nodeMap;

        for (llvm::Instruction &inst : instructions(f))
        {
          if (!isa<LoadInst>(&inst) && !isa<StoreInst>(&inst))
          {
            continue;
          }

          bool isRead = isa<LoadInst>(&inst);
          Value *rawPtr = nullptr;

          if (llvm::LoadInst *load = dyn_cast<LoadInst>(&inst))
          {
            rawPtr = load->getPointerOperand();
          }
          else if (llvm::StoreInst *store = dyn_cast<StoreInst>(&inst))
          {
            rawPtr = store->getPointerOperand();
          }

          Value *ptr = getUnderlyingObject(rawPtr);
          MemoryAccessNode *node = new MemoryAccessNode(ptr, isRead ? AccessType::Read : AccessType::Write);

          // Stride detection via scalar evolution
          if (rawPtr)
          {
            const SCEV *scev = SE.getSCEV(rawPtr);
            if (const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(scev))
            {
              if (AR->isAffine())
              {
                const SCEV *step = AR->getStepRecurrence(SE);
                if (const SCEVConstant *C = dyn_cast<SCEVConstant>(step))
                {
                  node->hasStride = true;
                  node->stride = static_cast<int>(C->getValue()->getSExtValue());
                }
              }
            }
            else if (const SCEVAddExpr *Add = dyn_cast<SCEVAddExpr>(scev))
            {
              for (const SCEV *op : Add->operands())
              {
                if (const SCEVMulExpr *Mul = dyn_cast<SCEVMulExpr>(op))
                {
                  for (const SCEV *mop : Mul->operands())
                  {
                    if (const SCEVConstant *C = dyn_cast<SCEVConstant>(mop))
                    {
                      int64_t val = C->getValue()->getSExtValue();
                      if (val > 0)
                      {
                        node->hasStride = true;
                        node->stride = static_cast<int>(val);
                      }
                    }
                  }
                }
              }
            }
          }

          nodeMap[&inst] = node;
          task.nodes.push_back(node);

          if (!task.entryNode)
          {
            task.entryNode = node;
          }

          auto iterF = [&](Value *src, DGEdge<Value, Value> *dep) -> bool
          {
            llvm::Instruction *srcInst = dyn_cast<Instruction>(src);
            llvm::Instruction *dstInst = dyn_cast<Instruction>(dep->getDst());

            if (!srcInst || !dstInst)
            {
              return false;
            }

            MemoryAccessNode *srcNode = nodeMap[srcInst];
            MemoryAccessNode *dstNode = nodeMap[dstInst];

            if (!srcNode || !dstNode)
            {
              return false;
            }

            auto E = std::make_unique<MemoryDependenceEdge>();
            E->src = srcNode;
            E->dst = dstNode;
            E->aliasType = isMustMemoryDep(dep) ? AliasType::Must : AliasType::May;

            srcNode->outgoing.push_back(E.get());
            dstNode->incoming.push_back(E.get());

            task.edges.push_back(std::move(E));
            return false;
          };

          FDG->iterateOverDependencesTo(
              &inst,
              false, // control deps
              true,  // memory deps
              false, // register deps
              iterF);
        }

        totalNodeCount += task.nodes.size();
        task.nodes = topologicalSort(task.nodes);
        printDeps(task, f);
        injectTaskDescriptor(M, f, emitTaskGlobal(M, task));
      }

      outs() << "NUMBER OF NODES: " << totalNodeCount << "\n";
      return true;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override
    {
      AU.addRequired<NoellePass>();
      AU.addRequired<ScalarEvolutionWrapperPass>();
    }
  };
}

char CAT::ID = 0;
static RegisterPass<CAT> X("OpenMPDependence", "Generate and embed dependency information into IR");

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