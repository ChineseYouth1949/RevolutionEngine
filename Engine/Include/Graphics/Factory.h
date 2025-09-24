#pragma once

#include <memory>

#include "Base/Export.h"
#include "Base/Check.h"

namespace RE::Graphics {

enum struct FactoryFlag { None = 0, Debug };

class RE_API Factory {
 public:
  Factory();

  ResultCode Build(FactoryFlag flag);

 private:
  struct Impl;
  std::shared_ptr<Impl> m_impl;
};

}  // namespace RE::Graphics