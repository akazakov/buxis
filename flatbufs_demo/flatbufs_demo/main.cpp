#include <fmt/format-inl.h>
#include <nanobench.h>

#include <random>

#include "schema_generated.h"
using namespace kazakov::buxis::flatbufs_demo;

struct NativeNode {
    double value;
    uint16_t feature;
    uint16_t left_child_offset;
    uint8_t leaf_node_flags;
    uint8_t inner_node_flags;
};

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
                 .leaf_node_flags = nodes[0].loaf_node_flags(),
                 .inner_node_flags = nodes[0].inner_node_flags()});

    for (int j = 1; j < kNumNodesPerTree; j++) {
      auto is_leaf = j * 2 + 2 >= kNumNodesPerTree ? LeafNodeFlags_IsLeaf : 0;
      nodes[j] = Node(urd(rnd), j, j * 2 + 1, is_leaf, 0);
      native_nodes.push_back(
        NativeNode{.value = nodes[j].value(),
                   .feature = nodes[j].feature(),
                   .left_child_offset = nodes[j].left_child_offset(),
                   .leaf_node_flags = nodes[j].loaf_node_flags(),
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
  auto bench = ankerl::nanobench::Bench().minEpochIterations(1000);

  auto test_v = std::vector<double>(kNumFeatures);
  for (auto& item : test_v) {
    item = urd(rnd);
  }

  double prediction1 = 0;
  bench.run("native", [&] {
    for (auto& tree : native_trees) {
      auto current_node = tree[0];
      while (!current_node.leaf_node_flags) {
        uint16_t child_ofs =
          test_v[current_node.feature] < current_node.value ? 0 : 1;
        current_node = tree[current_node.left_child_offset + child_ofs];
      }
      prediction1 += current_node.value;
    }

    return prediction1;
  });

  double prediction2 = 0;
  bench.run("flatbuf", [&] {
    auto numTrees = ensemble->trees()->size();
    auto trees_table = ensemble->trees();
    for (int i = 0; i < numTrees; i++) {
      auto current_tree = trees_table->Get(i);
      auto current_tree_nodes = current_tree->nodes();
      auto current_node = current_tree_nodes->Get(0);
      while (!current_node->loaf_node_flags()) {
        uint16_t child_ofs =
          test_v[current_node->feature()] < current_node->value() ? 0 : 1;
        current_node = current_tree_nodes->Get(
          current_node->left_child_offset() + child_ofs);
      }
      prediction2 += current_node->value();
    }
    return prediction2;
  });
  fmt::println("R1: {}", prediction1);
  fmt::println("R2: {}", prediction2);

  return 0;
}
