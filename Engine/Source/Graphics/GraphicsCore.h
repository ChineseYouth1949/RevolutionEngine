#pragma once

#include "Base/CompileDefine.h"
#include "Base/Result.h"

#include "Object/RObject.h"
#include "Utils/Utils.h"
#include "Scene/Scene.h"

#include "Graphics/Graphics.h"

namespace RE::Engine {

class GraphicsCore : public RObject {
 public:
  GraphicsCore();
  ~GraphicsCore();

  GraphicsCore(GraphicsCore&&) noexcept = default;
  GraphicsCore& operator=(GraphicsCore&&) noexcept = default;

  GraphicsCore(const GraphicsCore&) = delete;
  GraphicsCore& operator=(const GraphicsCore&) = delete;

  Result Initialize(Config* pConfig);

  Result BindScene(Scene* pScene);
  Result UnBindScne();

 private:
  GraphicsCore* mImpl;
};

}  // namespace RE::Engine