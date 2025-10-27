#pragma once

#include "Base/CompileDefine.h"

#include "Component.h"

#include <unordered_map>

namespace RE::Engine {

class System {
 public:
  private:
  std::unordered_map<Component::Handle, Component*> mComMap;
};

}  // namespace RE::Engine