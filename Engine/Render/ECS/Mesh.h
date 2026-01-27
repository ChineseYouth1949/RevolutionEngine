#pragma once

namespace re::engine::render {
template <typename VertexType, typename IndexType>
struct Mesh {
  vector<VertexType> vertexs;
  vector<IndexType> indexes;
};
}  // namespace re::engine::render