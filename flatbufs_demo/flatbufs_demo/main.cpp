#include <fmt/format-inl.h>

#include "schema_generated.h"
using namespace kazakov::buxis::flatbufs_demo;
int main() {
  size_t kNumTrees = 1000;
  size_t kNumNodesPerTree = 10'000;
  flatbuffers::FlatBufferBuilder builder;
  fmt::println("Sizeof(Node) = {}", sizeof(kazakov::buxis::flatbufs_demo::Node));

  std::vector<flatbuffers::Offset<Tree>> trees;
  trees.reserve(kNumTrees);
  for (int i = 0; i < kNumTrees; i++) {
    Node* nodes;
    uint16_t* depth;
    auto nodes_offset = builder.CreateUninitializedVectorOfStructs(kNumNodesPerTree, &nodes);
    for (int j = 0; j < kNumNodesPerTree; j++) {
      nodes[j] = Node(i * 0.13, j, j * 2 + 1, 0, 0);
    }
    auto depth_offset = builder.CreateUninitializedVector(kNumNodesPerTree, &depth);
    for (int j = 0; j < kNumNodesPerTree; j++) {
      depth[j] = j / 2;
    }
    trees.push_back(CreateTree(builder, nodes_offset, depth_offset));
    fmt::println("Current Size {}: {}", i, builder.GetSize());
  }
  auto trees_offset = builder.CreateVector(trees);
  auto ensemble_ofs = CreateEnsemble(builder, trees_offset);
  builder.Finish(ensemble_ofs);
  fmt::println("Final Size: {}", builder.GetSize());

  auto ensemble = GetEnsemble(builder.GetBufferPointer());
  auto trees_d = ensemble->trees();
  trees_d->size();
  fmt::println("Ensemble trees size: {}", trees_d->size());
  const Tree* tree1 = trees_d->Get(0);
  auto node1 = tree1->nodes();
  fmt::println("Tree[0] size: {}", tree1->nodes()->size());
  fmt::println("Tree[0] size: {}", tree1->nodes()->size());


  return 0;
}
