#include "Graphics/Factory.h"

#include "Base/D3D12.h"

namespace RE::Graphics {

struct Factory::Impl {
  ComPtr<IDXGIFactory4> factory;
  UINT falgs;
};

Factory::Factory() {}

void Factory::SetFlag(FactoryFlag flag) {
  switch (flag) {
    case FactoryFlag::Debug:
      m_impl->falgs |= DXGI_CREATE_FACTORY_DEBUG;
      break;
    default:
      break;
  }
}

RESULT Factory::Build() {
  return CreateDXGIFactory2(m_impl->falgs, IID_PPV_ARGS(&(m_impl->factory)));
}

}  // namespace RE::Graphics