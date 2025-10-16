#pragma once

#include <string>

namespace RE::Core {

class MaterialPropertyMap {
 public:
  static const std::string sShadingModel;
  static const std::string sMultiLayer;

  static const std::string sEmissive;
  static const std::string sEmissiveFactor;

  static const std::string sAmbient;
  static const std::string sAmbientFactor;

  static const std::string sDiffuse;
  static const std::string sDiffuseFactor;

  static const std::string sSpecular;
  static const std::string sSpecularFactor;
  static const std::string sShininess;

  static const std::string sBump;
  static const std::string sNormalMap;
  static const std::string sBumpFactor;

  static const std::string sTransparentColor;
  static const std::string sTransparencyFactor;

  static const std::string sReflection;
  static const std::string sReflectionFactor;

  static const std::string sDisplacementColor;
  static const std::string sDisplacementFactor;

  static const std::string sVectorDisplacementColor;
  static const std::string sVectorDisplacementFactor;
};

}  // namespace RE::Core