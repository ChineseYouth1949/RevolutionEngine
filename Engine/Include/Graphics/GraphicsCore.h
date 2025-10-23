#pragma once

#include "Base/Base.h"

namespace RE::Engine {
class Scene;

class RE_API GraphicsCore {
 public:
  GraphicsCore();
  ~GraphicsCore();

  GraphicsCore(GraphicsCore&&) noexcept = default;
  GraphicsCore& operator=(GraphicsCore&&) noexcept = default;

  GraphicsCore(const GraphicsCore&) = delete;
  GraphicsCore& operator=(const GraphicsCore&) = delete;

  Result BindScene(Scene* pScene);
  Result UnBindScne();

 private:
};

}  // namespace RE::Engine