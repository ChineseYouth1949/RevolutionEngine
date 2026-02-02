#pragma once

#include "Engine/Base/Macros.h"

namespace re::engine::ecs {
struct RE_API Command {
  virtual ~Command() = default;
  virtual void Execute() = 0;
};
}  // namespace re::engine::ecs