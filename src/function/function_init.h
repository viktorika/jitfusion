/*
 * @Author: victorika
 * @Date: 2025-01-23 10:37:20
 * @Last Modified by: victorika
 * @Last Modified time: 2026-02-09 14:48:41
 */
#include "function_registry.h"
#include "status.h"

namespace jitfusion {

Status InitMathInternalFunc(FunctionRegistry *reg);
Status InitStringInternalFunc(FunctionRegistry *reg);
Status InitListInternalFunc(FunctionRegistry *reg);

Status InitListArithmeticFunc(FunctionRegistry *reg);
Status InitListComparisonFunc(FunctionRegistry *reg);

}  // namespace jitfusion