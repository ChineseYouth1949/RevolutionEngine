#pragma once

#include <string>
#include <unordered_map>

#include "Base/Base.h"
#include "Math/Math.h"

#include "Texture.h"

namespace RE::Core {

enum struct PropertyType {
  None = 0,
  Vector4f,
  Texture,
};

class RE_API Material {
 public:
  bool HaveProperty(const std::string& pPropName);
  PropertyType GetPropertyType(const std::string& pPropName);

  const Vector4f* GetVector4f(const std::string& pPropName) const;
  const Texture* GetTexture(const std::string& pPropName) const;

  void SetProperty(const std::string& pPropName, const Vector4f& value);
  void SetProperty(const std::string& pPropName, const Texture* value);

 private:
  std::vector<std::pair<std::string, Vector3f>> m_valueVector3f;
  std::vector<std::pair<std::string, Texture*>> m_valueTexture;
};

}  // namespace RE::Core