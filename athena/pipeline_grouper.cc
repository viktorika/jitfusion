/*
 * @Author: victorika
 * @Date: 2026-03-30 19:23:34
 * @Last Modified by: victorika
 * @Last Modified time: 2026-03-31 15:14:46
 */
#include "pipeline_grouper.h"
#include <status.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "dsu.h"

namespace athena {

namespace {

inline uint64_t StringHash(const std::string& str) {
  uint64_t hash = 0;
  for (char c : str) {
    hash = hash * 131 + c;
  }
  return hash;
}

}  // namespace

std::vector<std::unique_ptr<ExecNode>> PipelineGrouper::Group(std::vector<std::unique_ptr<ExecNode>>& nodes) {
  if (nodes.empty()) {
    return {};
  }
  std::vector<uint64_t> root_ids;
  root_ids.reserve(nodes.size());
  for (size_t i = 0; i < nodes.size(); ++i) {
    current_root_id_ = StringHash("root_" + std::to_string(i));
    root_ids.push_back(current_root_id_);
    uint32_t height;
    GetHeight(nodes[i].get(), &height);
  }
  std::vector<std::unique_ptr<ExecNode>> result;
  std::unordered_map<uint64_t, std::vector<std::unique_ptr<ExecNode>>> union_id_to_nodes;
  for (size_t i = 0; i < nodes.size(); ++i) {
    uint64_t union_id = dsu_.Find(root_ids[i]);
    union_id_to_nodes[union_id].emplace_back(std::move(nodes[i]));
  }
  result.reserve(union_id_to_nodes.size());
  for (auto& [union_id, nodes] : union_id_to_nodes) {
    result.emplace_back(std::unique_ptr<ExecNode>(new NoOPNode(std::move(nodes))));
  }
  return result;
}

Status PipelineGrouper::GetHeight(ExecNode* exec_node, uint32_t* height) {
  JF_RETURN_NOT_OK(exec_node->Accept(this));
  if (1 == height_) {
    dsu_.Union(current_root_id_, StringHash(exec_node->ToString()));
  }
  *height = height_;
  return Status::OK();
}

Status PipelineGrouper::Visit(EntryArgumentNode& /*entry_argument_node*/) {
  height_ = 0;
  return Status::OK();
}

Status PipelineGrouper::Visit(ExecContextNode& /*exec_context_node*/) {
  height_ = 0;
  return Status::OK();
}

Status PipelineGrouper::Visit(OutputNode& /*output_node*/) {
  height_ = 0;
  return Status::OK();
}

Status PipelineGrouper::Visit(ConstantValueNode& /*const_node*/) {
  height_ = 0;
  return Status::OK();
}

Status PipelineGrouper::Visit(ConstantListValueNode& /*const_list_node*/) {
  height_ = 0;
  return Status::OK();
}

Status PipelineGrouper::Visit(UnaryOPNode& unary_op_node) {
  uint32_t height;
  JF_RETURN_NOT_OK(GetHeight(unary_op_node.GetChild(), &height));
  height_ = height + 1;
  return Status::OK();
}

Status PipelineGrouper::Visit(BinaryOPNode& binary_op_node) {
  uint32_t left_height;
  uint32_t right_height;
  JF_RETURN_NOT_OK(GetHeight(binary_op_node.GetLeft(), &left_height));
  JF_RETURN_NOT_OK(GetHeight(binary_op_node.GetRight(), &right_height));
  height_ = std::max(left_height, right_height) + 1;
  return Status::OK();
}

Status PipelineGrouper::Visit(FunctionNode& function_node) {
  uint32_t max_height = 0;
  for (const auto& arg : function_node.GetArgs()) {
    uint32_t height;
    JF_RETURN_NOT_OK(GetHeight(arg.get(), &height));
    max_height = std::max(max_height, height);
  }
  height_ = max_height + 1;
  return Status::OK();
}

Status PipelineGrouper::Visit(NoOPNode& no_op_node) {
  uint32_t max_height = 0;
  for (const auto& child : no_op_node.GetArgs()) {
    uint32_t height;
    JF_RETURN_NOT_OK(GetHeight(child.get(), &height));
    max_height = std::max(max_height, height);
  }
  height_ = max_height + 1;
  return Status::OK();
}

Status PipelineGrouper::Visit(IfNode& if_node) {
  uint32_t max_height = 0;
  for (const auto& child : if_node.GetArgs()) {
    uint32_t height;
    JF_RETURN_NOT_OK(GetHeight(child.get(), &height));
    max_height = std::max(max_height, height);
  }
  height_ = max_height + 1;
  return Status::OK();
}

Status PipelineGrouper::Visit(SwitchNode& switch_node) {
  uint32_t max_height = 0;
  for (const auto& child : switch_node.GetArgs()) {
    uint32_t height;
    JF_RETURN_NOT_OK(GetHeight(child.get(), &height));
    max_height = std::max(max_height, height);
  }
  height_ = max_height + 1;
  return Status::OK();
}

}  // namespace athena