#include "Graphics/IGraphicsCore.h"
#include "GraphicsCore.h"

namespace RE::Engine {

IGraphicsCore::IGraphicsCore() : mImpl(nullptr) {}
IGraphicsCore::~IGraphicsCore() {}

Result IGraphicsCore::Initialize(Config* pConfig) {
  Result lRes;
  return lRes;
}

Result IGraphicsCore::BindScene(Scene* pScene) {
  Result lRes;
  return lRes;
}
Result IGraphicsCore::UnBindScne() {
  Result lRes;
  return lRes;
}

}  // namespace RE::Engine