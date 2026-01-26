#pragma once

namespace re::engine::render {
template <typename VertexType, typename IndexType>
struct Mesh {
  Alloc::vector<VertexType> vertexs;
  Alloc::vector<IndexType> indexes;
};
}  // namespace re::engine::render