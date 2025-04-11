# athena

An execution engine utilizing DSL in combination with JITFusion.

# Build
Environment

  * jitfution
  * bazel 7.4.1

```bash
bazel build :athena
```
You can get the unit test binary use the following command.
```bash
bazel build :athena_test
```

# How to use
When using it, you need to <span style="color:red">#include "athena.h"</span> and depend on the <span style="color:red">Athena target</span>.

## DSL Rule

    1.The execution process of the entire program is determined by each statement, with each statement ending with a semicolon ";".
    2.The format of a statement must be constructed as follows: ${ID} = ${Expression}, where ID is understood as a variable name and Expression is an expression.
    3.In addition to supporting various operations, expressions also support several types of special syntax. Function syntax: ${Function_name}({$arg1}, {$arg2}...). It also supports switch statements and if statements. Following the principle of simplicity, the syntax for switch and if statements is similar to function syntax: if(${condition}, ${true_expression}, ${false_expression}), switch(${case1}, ${value1}, ${case2}, ${value2}..., ${default_value}).
    4.The ID of each statement must be unique to prevent errors.
    5.You can obtain the input parameter pointer through the entry_arg name and access the ExecContext via exec_ctx.
    6.The last statement will be returned, and any statements not depended upon by the last statement will not be executed. For example, if you have two statements: a = store(1); b = store(2), but neither a nor b is used by the root node or nodes dependent on the root node, then these statements will not be executed. (This is a temporary design and is expected to be modified in the future. There is already a direction for adjustment, but it will take time.)

For more details, please refer to the athena_test.cc file.