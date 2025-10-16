#include "Scene/MaterialPropMap.h"

#include <fbxsdk.h>

namespace RE::Core {

#define DefineMaterialProperty(x) const std::string MaterialPropertyMap::x = FbxSurfaceMaterial::x

DefineMaterialProperty(sShadingModel);
DefineMaterialProperty(sMultiLayer);

DefineMaterialProperty(sEmissive);
DefineMaterialProperty(sEmissiveFactor);

DefineMaterialProperty(sAmbient);
DefineMaterialProperty(sAmbientFactor);

DefineMaterialProperty(sDiffuse);
DefineMaterialProperty(sDiffuseFactor);

DefineMaterialProperty(sSpecular);
DefineMaterialProperty(sSpecularFactor);
DefineMaterialProperty(sShininess);

DefineMaterialProperty(sBump);
DefineMaterialProperty(sNormalMap);
DefineMaterialProperty(sBumpFactor);

DefineMaterialProperty(sTransparentColor);
DefineMaterialProperty(sTransparencyFactor);

DefineMaterialProperty(sReflection);
DefineMaterialProperty(sReflectionFactor);

DefineMaterialProperty(sDisplacementColor);
DefineMaterialProperty(sDisplacementFactor);

DefineMaterialProperty(sVectorDisplacementColor);
DefineMaterialProperty(sVectorDisplacementFactor);

}  // namespace RE::Core