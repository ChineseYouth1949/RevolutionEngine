#pragma once

#include "stdafx.h"

namespace Engine {

class GraphicCore {
 public:
  GraphicCore();

  void Init();

 private:
  ComPtr<IDXGISwapChain3> m_swapChain;
  ComPtr<ID3D12Device> m_device;

  bool m_useWarpDevice;
};

}  // namespace Engine