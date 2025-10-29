#pragma once

#include "GpuResouce.h"

namespace RE::Engine {

class UploadBuffer : public GpuResource {
 public:
  virtual ~UploadBuffer() { Destroy(); }

  void Create(ID3D12Device* device, const std::wstring& name, size_t BufferSize);

  void* Map();
  void UnMap(size_t begin = 0, size_t end = -1);

  size_t GetBufferSize() const { return mBufferSize; }

 private:
  size_t mBufferSize;
};

}  // namespace RE::Engine