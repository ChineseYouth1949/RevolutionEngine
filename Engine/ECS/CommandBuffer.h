#pragma once

#include "Engine/Base/Macros.h"

namespace re::engine::ecs {
class CommandQueue;

class RE_API CommandBuffer {
 public:
  CommandBuffer() = default;
  virtual ~CommandBuffer() = default;

 virtual Sumbit private : CommandQueue* m_queue;
};

}  // namespace re::engine::ecs