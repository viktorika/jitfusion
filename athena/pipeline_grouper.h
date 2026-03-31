/*
 * @Author: victorika
 * @Date: 2026-03-30 17:19:44
 * @Last Modified by: victorika
 * @Last Modified time: 2026-03-31 11:15:44
 */
#pragma once

#include "dsu.h"
#include "exec_node.h"

namespace athena {

using jitfusion::BinaryOPNode;
using jitfusion::ConstantListValueNode;
using jitfusion::ConstantValueNode;
using jitfusion::EntryArgumentNode;
using jitfusion::ExecContextNode;
using jitfusion::ExecNode;
using jitfusion::FunctionNode;
using jitfusion::IfNode;
using jitfusion::NoOPNode;
using jitfusion::OutputNode;
using jitfusion::Status;
using jitfusion::SwitchNode;
using jitfusion::UnaryOPNode;
using jitfusion::Visitor;

class PipelineGrouper : jitfusion::Visitor {
 public:
  PipelineGrouper() = default;
  ~PipelineGrouper() = default;

  // Merge the pipeline ASTs by category into new pipeline ASTs, where the root node of each new pipeline is a NoOp
  // node, and the original pipeline ASTs will be cleared.
  std::vector<std::unique_ptr<ExecNode>> Group(std::vector<std::unique_ptr<ExecNode>>& nodes);

  Status GetHeight(ExecNode* exec_node, uint32_t* height);

  Status Visit(EntryArgumentNode& entry_argument_node) override;
  Status Visit(ExecContextNode& exec_context_node) override;
  Status Visit(OutputNode& output_node) override;
  Status Visit(ConstantValueNode& const_node) override;
  Status Visit(ConstantListValueNode& const_list_node) override;
  Status Visit(UnaryOPNode& unary_op_node) override;
  Status Visit(BinaryOPNode& binary_op_node) override;
  Status Visit(FunctionNode& function_node) override;
  Status Visit(NoOPNode& no_op_node) override;
  Status Visit(IfNode& if_node) override;
  Status Visit(SwitchNode& switch_node) override;

 private:
  uint32_t height_{0};
  uint64_t current_root_id_{0};
  DSU dsu_;
};

}  // namespace athena