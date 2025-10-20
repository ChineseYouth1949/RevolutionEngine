#pragma once

#include <string>
#include <vector>

#include "Base/Base.h"
#include "Math/Math.h"

namespace RE::Engine {

struct RE_API Mesh {
  std::string m_name;

  std::vector<Vector3f> m_vertices;
  std::vector<Vector3f> m_normals;
  std::vector<Vector3f> m_tangents;
  std::vector<Vector3f> m_bitangents;
  std::vector<Vector2f> m_textureCoords;
  std::vector<uint32_t> m_indices;
};

}  // namespace RE::Engine