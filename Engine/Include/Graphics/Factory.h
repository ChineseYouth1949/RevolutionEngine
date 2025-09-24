#pragma once

#include <memory>

#include "Base/Export.h"
#include "Base/Error.h"

namespace RE::Graphics {

enum struct FactoryFlag { None = 0, Debug };

class RE_API Factory {
 public:
  Factory();

  void SetFlag(FactoryFlag flag);
  RESULT Build();

 private:
  struct Impl;
  std::shared_ptr<Impl> m_impl;
};

}  // namespace RE::Graphics