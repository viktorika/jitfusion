# athena

An execution engine utilizing DSL in combination with jitfusion.

# How to use
When using it, you need to <span style="color:red">#include "athena.h"</span> and depend on the <span style="color:red">athena target</span>.

## DSL Rule

* 1.The execution process of the entire program is determined by each statement, with each statement ending with a semicolon ";".

* 2.The format of a statement must be constructed as follows: {ID} = {Expression}, where ID is understood as a variable name and Expression is an expression.

* 3.In addition to supporting various operations, expressions also support several types of special syntax. Function syntax: {Function_name}({arg1}, {arg2}, ...). It also supports switch statements and if statements. Following the principle of simplicity, the syntax for switch and if statements is similar to function syntax: if({condition}, {true_expression}, {false_expression}), switch({case1}, {value1}, {case2}, {value2}..., {default_value}).

* 4.The ID of each statement must be unique to prevent errors.

* 5.You can obtain the input parameter pointer through the entry_arg name, access the ExecContext via exec_ctx and obtain the output parameter pointer through the output name.

## Execute Funtion
```c++
  // Applicable to simple scenarios, the program will not actually use a custom store function to write data. Instead,
  // the result will be returned, similar to expression scenarios.
  Status Compile(const std::string& code, const std::unique_ptr<FunctionRegistry>& func_registry);
  Status Execute(void* entry_arguments, RetType* result);

  // Applicable to complex scenarios where multiple pipelines are computed simultaneously. Each pipeline writes data
  // using a custom function, and results are not returned. This is similar to feature processing scenarios.
  Status Compile(const std::vector<std::string>& code, const std::unique_ptr<FunctionRegistry>& func_registry);
  Status Execute(void* entry_arguments, void* result);
```

The first set of functions returns a value through the last statement and requires that no write operations occur during the process; otherwise, it results in undefined behavior. The second set of functions can accept multiple sets of code, where the statement requires the user to define a custom store function to write data. The function will perform merge and optimization processing on all the code.

It is recommended to divide custom functions into two categories: read-only functions and store functions. Read-only functions can either be data loading functions or computation functions that generate an intermediate variable. It is not recommended for computation functions to directly modify parameter variables; instead, they should return the computation result by generating a new variable. In this case, you can set a read-only attribute for your function, as shown in the code below.

```c++
void ReadOnlyFunctionSetter(llvm::ExecutionEngine* /*engine*/, llvm::Module* /*m*/, llvm::Function* f) {
  f->setDoesNotThrow();
  f->setMemoryEffects(llvm::MemoryEffects::readOnly());
}

FunctionSignature sign("load", {ValueType::kPtr, ValueType::kI32}, ValueType::kU32);
FunctionStructure func_struct = {FunctionType::kCFunc, reinterpret_cast<void*>(LoadU32), nullptr,
                                   ReadOnlyFunctionSetter};
EXPECT_TRUE(func_registry->RegisterFunc(sign, func_struct).ok());
```

When store functions, it is recommended to set the OutputNode as a corresponding attribute, similar to the code below.

```c++

int32_t StoreF32(void* output, int32_t index, float value) {
  reinterpret_cast<float*>(output)[index] = value;
  return 0;
}

void StoreFunctionSetter(llvm::ExecutionEngine* /*engine*/, llvm::Module* /*m*/, llvm::Function* f) {
  f->setDoesNotThrow();
  f->addAttributeAtIndex(1, llvm::Attribute::get(f->getContext(), llvm::Attribute::NoAlias));
  f->addAttributeAtIndex(1, llvm::Attribute::get(f->getContext(), llvm::Attribute::NoCapture));
  f->setOnlyAccessesArgMemory();
}
```


For more details, please refer to the athena_test.cc file.