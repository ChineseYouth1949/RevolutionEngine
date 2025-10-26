#pragma once

#include "Base/CompileDefine.h"
#include "Base/Result.h"

#include "Object/RObject.h"

#include "Type.h"

namespace RE::Engine {
class Scene;
class Config;
class GraphicsCore;
class Camera;

class RE_DECLSPEC IGraphicsCore : public RObject {
 public:
  IGraphicsCore();
  ~IGraphicsCore();

  IGraphicsCore(IGraphicsCore&&) noexcept = default;
  IGraphicsCore& operator=(IGraphicsCore&&) noexcept = default;

  IGraphicsCore(const IGraphicsCore&) = delete;
  IGraphicsCore& operator=(const IGraphicsCore&) = delete;

  Result Initialize(const GraphicsCoreConfig& config);

  void Update();
  void Render();

  Camera* GetCamera();

  Result BindScene(Scene* pScene);
  Result UnBindScne();

 private:
  GraphicsCore* mImpl;
};

}  // namespace RE::Engine