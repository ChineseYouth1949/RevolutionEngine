#include "Adapter.h"

#include "D3D12.h"

namespace RE::Graphics {

struct Impl {
  ComPtr<IDXGIAdapter> m_adapter;
}

Adapter::Adapter() {
  m_impl = std::make_shared<Impl>();
}

}  // namespace RE::Graphics