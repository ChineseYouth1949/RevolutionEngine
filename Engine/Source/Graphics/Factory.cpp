#include "Graphics/Factory.h"

#include "Base/D3D12.h"

namespace RE::Graphics {

struct Factory::Impl {
  ComPtr<IDXGIFactory> factory;
};

Factory::Factory() {}

ResultCode Factory::Build(FactoryFlag flag) {
  return CreateDXGIFactory2(UINT(flag), IID_PPV_ARGS(&(m_impl->factory)));
}

}  // namespace RE::Graphics