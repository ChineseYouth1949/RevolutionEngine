#include "Scene/Material.h"

namespace RE::Engine {

bool Material::HaveProperty(const std::string& pPropName) {
  return GetPropertyType(pPropName) != PropertyType::None;
}

PropertyType Material::GetPropertyType(const std::string& pPropName) {
  PropertyType lResType = PropertyType::None;

  for (auto& [str, propType] : mPropertyType) {
    if (str == pPropName) {
      lResType = propType;
      break;
    }
  }

  return lResType;
}

bool Material::SetProperty(const std::string& pPropName, const Vector3f& value) {
  PropertyType lPropType = GetPropertyType(pPropName);
  bool lRes = false;

  if (lPropType != PropertyType::None && lPropType != PropertyType::Vector3f) {
    for (auto& [str, vector3f] : mProperVector3f) {
      if (str == pPropName) {
        vector3f = value;
        lRes = true;
        break;
      }
    }
  }

  return lRes;
}
bool Material::GetProperty(const std::string& pPropName, Vector3f& pVlaueRes) {
  if (GetPropertyType(pPropName) != PropertyType::Vector3f) {
    return false;
  }

  for (auto& [str, vector3f] : mProperVector3f) {
    if (str == pPropName) {
      pVlaueRes = vector3f;
      return true;
    }
  }

  AssertInfo(0, "Not found property!");
}

bool Material::SetProperty(const std::string& pPropName, const Vector4f& value) {
  PropertyType lPropType = GetPropertyType(pPropName);
  bool lRes = false;

  if (lPropType != PropertyType::None && lPropType != PropertyType::Vector4f) {
    for (auto& [str, vector4f] : mProperVector4f) {
      if (str == pPropName) {
        vector4f = value;
        lRes = true;
        break;
      }
    }
  }

  return lRes;
}
bool Material::GetProperty(const std::string& pPropName, Vector4f& pVlaueRes) {
  if (GetPropertyType(pPropName) != PropertyType::Vector4f) {
    return false;
  }

  for (auto& [str, vector4f] : mProperVector4f) {
    if (str == pPropName) {
      pVlaueRes = vector4f;
      return true;
    }
  }

  AssertInfo(0, "Not found property!");
}

bool Material::SetProperty(const std::string& pPropName, Texture* value) {
  PropertyType propType = GetPropertyType(pPropName);
  bool lRes = false;

  if (propType != PropertyType::None && propType != PropertyType::Texture) {
    for (auto& [str, texture] : mProperTexture) {
      if (str == pPropName) {
        texture = value;
        lRes = true;
      }
    }
  }

  return lRes;
}
bool Material::GetProperty(const std::string& pPropName, Texture*& pValueRes) {
  if (GetPropertyType(pPropName) != PropertyType::Texture) {
    return false;
  }

  for (auto& [str, texture] : mProperTexture) {
    if (str == pPropName) {
      pValueRes = texture;
      return true;
    }
  }

  AssertInfo(0, "Not found property!");
}

}  // namespace RE::Engine