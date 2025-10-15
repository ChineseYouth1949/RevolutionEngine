// #pragma once

// #include <vector>

// #include "D3D12.h"

// namespace RE::Core {

// class RE_API GraphicsEngineImpl {
//  public:
//   GraphicsEngineImpl();
//   virtual ~GraphicsEngineImpl();

//   GraphicsEngineImpl(GraphicsEngineImpl&&) noexcept = default;
//   GraphicsEngineImpl& operator=(GraphicsEngineImpl&&) noexcept = default;

//   GraphicsEngineImpl(const GraphicsEngineImpl&) = delete;
//   GraphicsEngineImpl& operator=(const GraphicsEngineImpl&) = delete;

//   Result ConfigOptions();
//   Result Build();

//   Result LoadScene();

//  private:
//   ComPtr<ID3D12Device> m_device;
//   ComPtr<IDXGISwapChain3> m_swapChain;

//   uint8_t m_frameCount;
//   std::vector<FrameResource> m_frameResouces;
// };

// }  // namespace RE::Core