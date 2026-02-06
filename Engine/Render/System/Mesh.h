#pragma once

namespace re::engine::render {
template <typename VertexType, typename IndexType>
struct Mesh {
  vector<VertexType> vertexs;
  vector<IndexType> indexes;

  static constexpr size_t s_VertexSize = sizeof(VertexType);
  static constexpr size_t s_IndexSize = sizeof(IndexType);
};

template <typename VertexType>
struct SimpleMesh {
  vector<VertexType> vertexs;

  static constexpr size_t s_VertexSize = sizeof(VertexType);
};
}  // namespace re::engine::render