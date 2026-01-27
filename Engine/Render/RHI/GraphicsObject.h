#pragma once

#include "Macros.h"

namespace re::engine::render {
class GraphicsCore;

class GraphicsObject {
 public:
  GraphicsObject(GraphicsCore* gc = nullptr) { m_GC = gc; }

  RE_FINLINE void SetGraphicsCore(GraphicsCore* gc) { m_GC = gc; }
  RE_FINLINE GraphicsCore* GetGraphicsCore() { return m_GC; }
  RE_FINLINE bool HaveGraphicsCore() { return GetGraphicsCore() != nullptr; }

 private:
  static inline GraphicsCore* m_GC = nullptr;
};

#define RE_GCDevice GetGraphicsCore()->GetDevice()
}  // namespace re::engine::render