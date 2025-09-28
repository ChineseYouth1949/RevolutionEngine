#pragma once

#include <memory>

namespace RE::Graphics {

class Adapter {
 public:
  Adapter();

 private:
  struct Impl;
  std::shared_ptr<Impl> m_impl;
};

}  // namespace RE::Graphics