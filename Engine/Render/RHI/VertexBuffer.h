#pragma once

#include "GraphicsObject.h"
#include "GpuResource.h"

namespace re::engine::render {

class VertexBuffer : public GpuResource {
 public:
  virtual ~VertexBuffer() { Release(); }

  void Initialize(const std::wstring& name, size_t BufferSize);

  void Update(const void* data, size_t dataSize);

  size_t GetBufferSize() const { return m_BufferSize; }

 protected:
  size_t m_BufferSize;
  ComPtr<ID3D12Resource> m_UploadRes;
};

}  // namespace re::engine::render