#include <fmt/format-inl.h>
#include <nanobench.h>

#include <iostream>
#include <iprof/iprof.hpp>
#include <random>

#include "schema_generated.h"
using namespace kazakov::buxis::flatbufs_demo;

struct NativeNode {
    double value;
    uint16_t feature;
    uint16_t left_child_offset;
    uint8_t leaf_node_flags;
    uint8_t inner_node_flags;
    int16_t padding0__;
};

void FlatbufPredict(const std::vector<double>& test_v, unsigned int num_trees,
                    const std::vector<const Node*>& node_starts,
                    double& prediction2);
void NativePredict(const std::vector<std::vector<NativeNode>>& native_trees,
                   const std::vector<double>& test_v, double& prediction1);
int main() {
  size_t kNumTrees = 1000;
  size_t kNumFeatures = 1000;
  size_t kNumNodesPerTree = 10'000;
  flatbuffers::FlatBufferBuilder builder;
  std::mt19937_64 rnd(1);
  std::uniform_real_distribution<double> urd(-100, 100);
  std::uniform_real_distribution<double> feature_rnd(0, kNumFeatures);
  fmt::println("Sizeof(Node) = {}",
               sizeof(kazakov::buxis::flatbufs_demo::Node));

  std::vector<flatbuffers::Offset<Tree>> trees;
  std::vector<std::vector<NativeNode>> native_trees(kNumTrees);
  trees.reserve(kNumTrees);
  for (int i = 0; i < kNumTrees; i++) {
    Node* nodes;
    uint16_t* depth;
    auto nodes_offset =
      builder.CreateUninitializedVectorOfStructs(kNumNodesPerTree, &nodes);
    auto& native_nodes = native_trees[i];
    native_nodes.reserve(kNumNodesPerTree);
    nodes[0] = Node(urd(rnd), static_cast<uint16_t>(feature_rnd(rnd)), 1, 0, 0);
    native_nodes.push_back(
      NativeNode{.value = nodes[0].value(),
                 .feature = nodes[0].feature(),
                 .left_child_offset = nodes[0].left_child_offset(),
                 .leaf_node_flags = nodes[0].leaf_node_flags(),
                 .inner_node_flags = nodes[0].inner_node_flags()});

    for (int j = 1; j < kNumNodesPerTree; j++) {
      auto is_leaf = j * 2 + 2 >= kNumNodesPerTree ? LeafNodeFlags_IsLeaf : 0;
      nodes[j] = Node(urd(rnd), j, j * 2 + 1, is_leaf, 0);
      native_nodes.push_back(
        NativeNode{.value = nodes[j].value(),
                   .feature = nodes[j].feature(),
                   .left_child_offset = nodes[j].left_child_offset(),
                   .leaf_node_flags = nodes[j].leaf_node_flags(),
                   .inner_node_flags = nodes[j].inner_node_flags()});
    }

    auto depth_offset =
      builder.CreateUninitializedVector(kNumNodesPerTree, &depth);
    for (int j = 0; j < kNumNodesPerTree; j++) {
      depth[j] = j / 2;
    }
    trees.push_back(CreateTree(builder, nodes_offset, depth_offset));
    //    fmt::println("Current Size {}: {}", i, builder.GetSize());
  }
  auto trees_offset = builder.CreateVector(trees);
  auto ensemble_ofs = CreateEnsemble(builder, trees_offset);
  builder.Finish(ensemble_ofs);
  fmt::println("Final Size: {}Mb", builder.GetSize() / 1024 / 1024.0);

  auto ensemble = GetEnsemble(builder.GetBufferPointer());
  auto trees_d = ensemble->trees();
  trees_d->size();
  fmt::println("Ensemble trees size: {}", trees_d->size());
  const Tree* tree1 = trees_d->Get(0);
  fmt::println("Tree[0] size: {}", tree1->nodes()->size());
  auto bench = ankerl::nanobench::Bench()
                 .epochIterations(1000)
                 .epochs(10)
                 .relative(true)
                 .warmup(10);

  auto test_v = std::vector<double>(kNumFeatures);
  for (auto& item : test_v) {
    item = urd(rnd);
  }

  double prediction1 = 0;
  for (auto& tree : native_trees) {
    auto current_node = tree[0];
    while (!current_node.leaf_node_flags) {
      uint16_t child_ofs =
        test_v[current_node.feature] < current_node.value ? 0 : 1;
      current_node = tree[current_node.left_child_offset + child_ofs];
    }
    prediction1 += current_node.value;
  }

  double prediction2 = 0;
  auto num_trees = ensemble->trees()->size();
  auto trees_table = ensemble->trees();
  for (int i = 0; i < num_trees; i++) {
    auto current_tree = trees_table->Get(i);
    auto current_tree_nodes = reinterpret_cast<const Node*>(
      current_tree->nodes()->GetStructFromOffset(0));
    auto current_node = &current_tree_nodes[0];
    while (!current_node->leaf_node_flags()) {
      uint16_t child_ofs =
        test_v[current_node->feature()] < current_node->value() ? 0 : 1;
      current_node =
        &current_tree_nodes[current_node->left_child_offset() + child_ofs];
    }
    prediction2 += current_node->value();
  }
  fmt::println("R1: {}", prediction1);
  fmt::println("R2: {}", prediction2);

  auto tree = native_trees[0];
  bench.run("native", [&] {
    NativePredict(native_trees, test_v, prediction1);
    return prediction1;
  });

  std::vector<const Node*> node_starts(num_trees);
  for (int i = 0; i < num_trees; i++) {
    auto current_tree = trees_table->Get(i);
    auto current_tree_nodes = reinterpret_cast<const Node*>(
      current_tree->nodes()->GetStructFromOffset(0));
    node_starts[i] = current_tree_nodes;
  }

  bench.run("flatbuf", [&] {
    FlatbufPredict(test_v, num_trees, node_starts, prediction2);
    return prediction2;
  });
  fmt::println("R1: {}", prediction1);
  fmt::println("R2: {}", prediction2);

  InternalProfiler::aggregateEntries();
  std::cout << "\nThe profiler stats after the second run:\n"
            << InternalProfiler::stats << std::endl;
  return 0;
}

[[gnu::noinline]]
void NativePredict(const std::vector<std::vector<NativeNode>>& native_trees,
                   const std::vector<double>& test_v, double& prediction1) {
  IPROF_FUNC;
  for (auto& tree : native_trees) {
    auto current_node = &tree[0];
    while (!current_node->leaf_node_flags) {
      uint16_t child_ofs =
        test_v[current_node->feature] < current_node->value ? 0 : 1;
      current_node = &tree[current_node->left_child_offset + child_ofs];
    }
    prediction1 += current_node->value;
  }
}

[[gnu::noinline]]
void FlatbufPredict(const std::vector<double>& test_v, unsigned int num_trees,
                    const std::vector<const Node*>& node_starts,
                    double& prediction2) {
  IPROF_FUNC;
  for (int i = 0; i < num_trees; i++) {
    auto current_node = node_starts[i];
    while (!current_node->leaf_node_flags()) {
      uint16_t child_ofs =
        test_v[current_node->feature()] < current_node->value() ? 0 : 1;
      current_node =
        &node_starts[i][current_node->left_child_offset() + child_ofs];
    }
    prediction2 += current_node->value();
  }
}
