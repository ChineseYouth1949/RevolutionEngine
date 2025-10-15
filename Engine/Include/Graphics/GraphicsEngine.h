#pragma once

#include "Base/Base.h"

namespace RE::Core {

class RE_API GraphicsEngine {
 public:
  GraphicsEngine();
  ~GraphicsEngine();

  GraphicsEngine(GraphicsEngine&&) noexcept = default;
  GraphicsEngine& operator=(GraphicsEngine&&) noexcept = default;

  GraphicsEngine(const GraphicsEngine&) = delete;
  GraphicsEngine& operator=(const GraphicsEngine&) = delete;

  Result Build();

 private:
};

}  // namespace RE::Core