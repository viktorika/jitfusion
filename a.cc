#include <llvm/ADT/APFloat.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/raw_ostream.h>
#include <math.h>
#include <iostream>
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
// #include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
// #include "llvm/Support/DynamicLibrary.h"
#include "llvm/IR/Intrinsics.h"
// #include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"

using namespace llvm;

extern "C" double test_custom_log2(double a) { return a + 2; }

int main() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();
  llvm::InitializeNativeTargetDisassembler();
  // llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);

  LLVMContext Context;
  Module *M = new Module("test", Context);
  IRBuilder<> Builder(Context);

  // 创建函数类型：double(double)
  FunctionType *FuncTy = FunctionType::get(Type::getDoubleTy(Context), {Type::getDoubleTy(Context)}, false);

  // 创建log函数
  llvm::FunctionCallee LogFunc = M->getOrInsertFunction("custom_log2", FuncTy);

  //Function *LogFunc = Function::Create(FuncTy, Function::ExternalLinkage, "custom_log2", M);
  // 如果只需要声明，则后面的实现可以不要，外链到自定义的函数
  // BasicBlock *LogEntry = BasicBlock::Create(Context, "entry", LogFunc);
  // Builder.SetInsertPoint(LogEntry);

  // // 实现log函数
  // // 这里我们简单地使用LLVM提供的log指令实现log函数
  // // 在实际应用中，您可能需要使用更复杂的算法来实现log函数
  // Value *Arg = LogFunc->arg_begin();
  // Value *LogValue = Builder.CreateUnaryIntrinsic(Intrinsic::log2, Arg);
  // Builder.CreateRet(LogValue);

  // 创建主函数
  Function *MainFunc =
      Function::Create(FunctionType::get(Type::getDoubleTy(Context),
                                         {llvm::PointerType::get(llvm::Type::getDoubleTy(Context), 0)}, false),
                       Function::ExternalLinkage, "test_main", M);
  BasicBlock *BB = BasicBlock::Create(Context, "entry", MainFunc);
  Builder.SetInsertPoint(BB);

  // 调用log函数
  // const DataLayout& DL = M->getDataLayout();
  Value *log_args = MainFunc->arg_begin();
  Value *arg1 =
      Builder.CreateLoad(Type::getDoubleTy(Context),
                         Builder.CreateBitCast(log_args, llvm::PointerType::get(llvm::Type::getDoubleTy(Context), 0)));
  // Value* arg1 = Builder.CreateLoad(Builder.CreateBitCast(log_args,
  // Type::getDoublePtrTy(Context)),
  //                                  DL.getPrefTypeAlignment(Type::getDoublePtrTy(Context)));
  Value *Result = Builder.CreateCall(LogFunc, arg1);

  // 返回0
  Builder.CreateRet(Result);

  // 验证生成的代码，检查是否有错误
  if (llvm::verifyModule(*M, &errs())) {
    M->print(errs(), nullptr);
    return 1;
  }
  M->print(errs(), nullptr);
  // 创建执行引擎
  std::string ErrorStr;
  ExecutionEngine *EE =
      EngineBuilder(std::unique_ptr<Module>(M)).setEngineKind(EngineKind::JIT).setErrorStr(&ErrorStr).create();
  if (EE == nullptr) {
    llvm::errs() << "ee create fail: " << ErrorStr << "\n";
    return 1;
  }
  // std::cout << "pre addr=" << EE->getFunctionAddress("custom_log2") << std::endl;
  // EE->addGlobalMapping("custom_log2", (uint64_t)(uintptr_t)&test_custom_log2);
  auto *f = M->getFunction("custom_log2");
  EE->addGlobalMapping(f, (void *)test_custom_log2);
  EE->finalizeObject();
  std::cout << "after addr=" << EE->getFunctionAddress("custom_log2") << std::endl;
  // 创建一个GenericValue来存储参数
  // GenericValue arg;
  // arg.DoubleVal = 2.0;  // 假设我们传递的参数是2.0

  // 执行主函数，传递参数
  // std::vector<GenericValue> main_args = {arg};
  // GenericValue gv = EE->runFunction(MainFunc, main_args);
  // std::cout << "value=" << gv.DoubleVal << std::endl;
  uint64_t main_func_addr = EE->getFunctionAddress("test_main");
  typedef double (*MainFuncType)(void *);
  auto mainfunc = (MainFuncType)(main_func_addr);
  double x = 14.0;
  void *x_ptr = &x;
  std::cout << "result=" << mainfunc(x_ptr);

  // 清理
  delete EE;
  llvm_shutdown();

  return 0;
}