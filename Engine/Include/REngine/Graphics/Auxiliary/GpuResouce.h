#pragma once

#include "Graphics/Public/D3D12.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace RE::Engine {

class GpuResource {
 public:
  GpuResource()
      : mGpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL),
        mUsageState(D3D12_RESOURCE_STATE_COMMON),
        mTransitioningState((D3D12_RESOURCE_STATES)-1) {}

  GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState)
      : mGpuVirtualAddress(D3D12_GPU_VIRTUAL_ADDRESS_NULL),
        mResource(pResource),
        mUsageState(CurrentState),
        mTransitioningState((D3D12_RESOURCE_STATES)-1) {}

  ~GpuResource() { Destroy(); }

  virtual void Destroy() {
    mResource = nullptr;
    mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
    ++mVersionId;
  }

  ID3D12Resource* operator->() { return mResource.Get(); }
  const ID3D12Resource* operator->() const { return mResource.Get(); }

  ID3D12Resource* GetResource() { return mResource.Get(); }
  const ID3D12Resource* GetResource() const { return mResource.Get(); }

  uint32_t GetVersionID() const { return mVersionId; }
  ID3D12Resource** GetAddressOf() { return mResource.GetAddressOf(); }
  D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return mGpuVirtualAddress; }

 protected:
  ComPtr<ID3D12Resource> mResource;
  D3D12_RESOURCE_STATES mUsageState;
  D3D12_RESOURCE_STATES mTransitioningState;
  D3D12_GPU_VIRTUAL_ADDRESS mGpuVirtualAddress;

  uint32_t mVersionId = 0;
};

}  // namespace RE::Engine