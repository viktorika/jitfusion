# jitfusion
A computing engine completely based on llvm


# Build
First, you need to have LLVM 19.1.7; other versions will not work. However, if you can only use other versions, you can modify the exec_engine.cc file to adapt it, and it shouldn't require too many changes. Additionally, currently, only Bazel is supported for compilation.

You can use the following command to check your version.
```bash
llvm-config --version
```

You can get the library use the following command.
```bash
bazel build //:jitfusion
```
You can get the unit test binary use the following command.
```bash
bazel build //:jitfusion_test
```

# How to use
I considered how many types of nodes are needed to represent a function in the execution engine, and I ultimately abstracted it into 10 types of nodes:

    EntryArgumentNode: A node used to obtain the entry argument variables of a function.

    ExecContextNode: A node used to obtain the context variables of the execution engine.

    ConstValueNode and ConstListValueNode: Constant nodes.

    UnaryOPNode: Unary operation node.

    BinaryOPNode: Binary operation node.

    FunctionNode: Function node.

    IfNode: If condition node.

    SwitchNode: Switch condition node.

    NoOPNode: No operation node.


For example, generally speaking, the process of an execution flow graph might look like this.


<div align=center><img src="doc/exec_pipeline_example.png" width="50%" height="50%"></div>
</br>

Typically, the initial node is a data loading node. After a series of operations in the middle to obtain the desired value, the process is completed with a data storing node at the end.

The data reading and writing nodes can both be implemented using EntryArgumentNode and FunctionNode. You can refer to test/entry_argument_node_test.cc for more details. for example:

```c++
int32_t LoadValue(int64_t data, uint32_t i) { return reinterpret_cast<int32_t*>(data)[i]; }

int main() {
  std::unique_ptr<FunctionRegistry> func_registry;
  FunctionRegistryFactory::CreateFunctionRegistry(&func_registry);
  FunctionSignature sign("load", {ValueType::kI64, ValueType::kI32}, ValueType::kI32);
  FunctionStructure func_struct = {FunctionType::kCFunc, reinterpret_cast<void*>(LoadValue), nullptr};
  func_registry->RegisterFunc(sign, func_struct);

  auto args_node = std::unique_ptr<ExecNode>(new EntryArgumentNode);
  auto index_node = std::unique_ptr<ExecNode>(new ConstantValueNode(1));
  std::vector<std::unique_ptr<ExecNode>> load_func_args;
  load_func_args.emplace_back(std::move(args_node));
  load_func_args.emplace_back(std::move(index_node));
  auto load_func_node = std::unique_ptr<ExecNode>(new FunctionNode("load", std::move(load_func_args)));

  ExecEngine exec_engine;
  auto st = exec_engine.Compile(load_func_node, func_registry);
  RetType result;
  std::vector<int32_t> data_list = {100, 200, 300, 400};
  exec_engine.Execute(data_list.data(), &result);
}
```

The EntryArgumentNode will consistently return a u64 value, which is the input parameter for the execution engine's Execute function. By passing this value to a custom function, you can perform operations on this parameter.If you want to achieve maximum performance, you can implement the codegen function. You can refer to the implementations in the src/function directory for guidance.

The intermediate processes can all be converted into corresponding op nodes, function nodes, condition nodes, etc. Additionally, there are usually multiple execution flows within a single task, and these flows may use the same variables. To achieve maximum optimization, you can have all the store nodes ultimately point to a NoOP node, allowing LLVM to perform the optimization for you.






