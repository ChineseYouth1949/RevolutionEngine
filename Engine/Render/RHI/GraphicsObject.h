#pragma once

#include "Macros.h"

namespace re::engine::render {
class GraphicsCore;

class GraphicsObject {
 public:
  GraphicsObject(GraphicsCore* gc = nullptr) { m_GC = gc; }

  RE_FINLE void SetGraphicsCore(GraphicsCore* gc) { m_GC = gc; }
  RE_FINLE GraphicsCore* GetGraphicsCore() { return m_GC; }
  RE_FINLE bool HaveGraphicsCore() { return GetGraphicsCore() != nullptr; }

 private:
  GraphicsCore* m_GC = nullptr;
};

#define RE_GCDevice GetGraphicsCore()->GetDevice()
}  // namespace re::engine::render