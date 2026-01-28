#pragma once

namespace re::engine::render {
template <typename VertexType, typename IndexType>
struct Mesh {
  vector<VertexType> vertexs;
  vector<IndexType> indexes;

  static constexpr size_t s_VertexSize = sizeof(VertexType);
  static constexpr size_t s_IndexSize = sizeof(IndexType);
};
}  // namespace re::engine::render