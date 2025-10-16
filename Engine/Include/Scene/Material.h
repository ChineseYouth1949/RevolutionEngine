#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Base/Base.h"
#include "Math/Math.h"

#include "Texture.h"

namespace RE::Core {

enum struct PropertyType {
  None = 0,
  Vector3f,
  Vector4f,
  Texture,
};

class RE_API Material {
 public:
  bool HaveProperty(const std::string& pPropName);
  PropertyType GetPropertyType(const std::string& pPropName);

  bool SetProperty(const std::string& pPropName, const Vector3f& value);
  bool SetProperty(const std::string& pPropName, const Vector4f& value);
  bool SetProperty(const std::string& pPropName, Texture* value);

  bool GetProperty(const std::string& pPropName, Vector3f& pVlaueRes);
  bool GetProperty(const std::string& pPropName, Vector4f& pVlaueRes);
  bool GetProperty(const std::string& pPropName, Texture*& pValueRes);

 private:
  std::vector<std::pair<std::string, PropertyType>> mPropertyType;

  std::vector<std::pair<std::string, Vector3f>> mProperVector3f;
  std::vector<std::pair<std::string, Vector4f>> mProperVector4f;
  std::vector<std::pair<std::string, Texture*>> mProperTexture;
};

}  // namespace RE::Core