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

 private:
  Result BindScene();
  Result UnBindScne();
};

}  // namespace RE::Engine