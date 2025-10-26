#include "Graphics/IGraphicsCore.h"

#include "GraphicsCore.h"

#include "Memory/Memory.h"

namespace RE::Engine {

IGraphicsCore::IGraphicsCore() : mImpl(GMemoryAllocator::Create<GraphicsCore>()) {}
IGraphicsCore::~IGraphicsCore() {
  if (mImpl) {
    GMemoryAllocator::Destry(mImpl);
    mImpl = nullptr;
  }
}

Result IGraphicsCore::Initialize(const GraphicsCoreConfig& config) {
  Result lRes;

  lRes = mImpl->Initialize(config);

  return lRes;
}

void IGraphicsCore::Update() {
  mImpl->Update();
}

void IGraphicsCore::Render() {
  mImpl->Render();
}

Camera* IGraphicsCore::GetCamera() {
  return mImpl->GetCamera();
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