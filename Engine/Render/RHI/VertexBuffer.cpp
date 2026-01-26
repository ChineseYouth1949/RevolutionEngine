#include "GraphicsCore.h"
#include "VertexBuffer.h"

using namespace re::engine::render;

void VertexBuffer::Initialize(const std::wstring& name, size_t BufferSize) {
  Release();
  auto HeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
  auto BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(BufferSize);
  RE_ASSERT_SUCCEEDED(RE_GCDevice->CreateCommittedResource(&HeapProp, D3D12_HEAP_FLAG_NONE, &BufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                                                           IID_PPV_ARGS(&m_pResource)));
}

void VertexBuffer::Update(const void* data, size_t dataSize) {
  // 1. 修正上传堆状态
  auto HeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
  auto BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(dataSize);

  RE_ASSERT_SUCCEEDED(RE_GCDevice->CreateCommittedResource(&HeapProp, D3D12_HEAP_FLAG_NONE, &BufferDesc,
                                                           D3D12_RESOURCE_STATE_GENERIC_READ,  // 必须是此状态
                                                           nullptr, IID_PPV_ARGS(&m_UploadRes)));

  // 2. 映射并拷贝
  void* pData;
  m_UploadRes->Map(0, nullptr, &pData);
  memcpy(pData, data, dataSize);
  m_UploadRes->Unmap(0, nullptr);

  auto commandList = GetGraphicsCore()->GetCommandList();

  // 3. 状态切换（假设从上次的读取状态切回写入状态）
  auto b1 = CD3DX12_RESOURCE_BARRIER::Transition(m_pResource.Get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST);
  commandList->ResourceBarrier(1, &b1);

  commandList->CopyBufferRegion(m_pResource.Get(), 0, m_UploadRes.Get(), 0, dataSize);

  // 4. 切换回读取状态
  auto b2 = CD3DX12_RESOURCE_BARRIER::Transition(m_pResource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
  commandList->ResourceBarrier(1, &b2);
}