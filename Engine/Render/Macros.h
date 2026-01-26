#pragma once

#define D3D12_GPU_VIRTUAL_ADDRESS_NULL ((D3D12_GPU_VIRTUAL_ADDRESS)0)
#define D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN ((D3D12_GPU_VIRTUAL_ADDRESS) - 1)

#ifdef RE_DEBUG
#define RE_D3D12_SetName(obj, name) \
  do {                              \
    if ((obj) != nullptr) {         \
      (obj)->SetName(name);         \
    }                               \
  } while (0)
#else
#define RE_D3D12_SetName(obj, name) (void)0
#endif

namespace re::engine::render {
using namespace Microsoft::WRL;
}