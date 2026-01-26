#include "Engine/Core/Core.h"

#include "Engine/Render/RHI/GraphicsCore.h"
#include "Engine/Render/RHI/VertexBuffer.h"

#include "ColorVertex.h"
#include "Mesh.h"

using namespace re::engine::render;
using namespace re::engine::ecs;
using namespace re::engine;

struct EntityData {
  VertexBuffer vertexBuffer;
};

struct RenderColorVertex::Impl {
  GraphicsCore* gc;
  Alloc::UniquePtr<RootSignature> signature;
  Alloc::wstring vertexShaderName;
  Alloc::wstring fragmentShaderName;
  GraphicsPSO pso;
  Alloc::unordered_map<EntityId, EntityData*> entityDataMap;
};

RenderColorVertex::RenderColorVertex(GraphicsCore* gc) {
  m_Impl = Alloc::CreateUniquePtr<Impl>();
  m_Impl->gc = gc;
}
RenderColorVertex::~RenderColorVertex() {}

bool RenderColorVertex::Initialize() {
  // Create an empty root signature.
  {
    m_Impl->signature = Alloc::CreateUniquePtr<RootSignature>(0, 0);
    m_Impl->signature->SetGraphicsCore(m_Impl->gc);
    m_Impl->signature->Finalize(L"Empty RootSignature");
  }

  // Create the pipeline state, which includes compiling and loading shaders.
  {
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    RE_ASSERT_SUCCEEDED(D3DCompileFromFile(m_Impl->gc->GetAssetFullPath(m_Impl->vertexShaderName).c_str(), nullptr, nullptr, "VSMain", "vs_5_0",
                                           compileFlags, 0, &vertexShader, nullptr));
    RE_ASSERT_SUCCEEDED(D3DCompileFromFile(m_Impl->gc->GetAssetFullPath(m_Impl->fragmentShaderName).c_str(), nullptr, nullptr, "PSMain", "ps_5_0",
                                           compileFlags, 0, &pixelShader, nullptr));

    m_Impl->pso = GraphicsPSO(L"ColorVertex_PSO");
    m_Impl->pso.SetBlendState(CD3DX12_BLEND_DESC(D3D12_DEFAULT));
    m_Impl->pso.SetRasterizerState(CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT));
    m_Impl->pso.SetDepthStencilState(CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT));
    m_Impl->pso.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    m_Impl->pso.SetRenderTargetFormat(m_Impl->gc->GetSwapChainForamt(), DXGI_FORMAT_D24_UNORM_S8_UINT);

    m_Impl->pso.SetVertexShader(vertexShader->GetBufferPointer(), vertexShader->GetBufferSize());
    m_Impl->pso.SetPixelShader(pixelShader->GetBufferPointer(), pixelShader->GetBufferSize());

    // Define the vertex input layout.
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};
    m_Impl->pso.SetInputLayout(2, inputElementDescs);

    m_Impl->pso.Finalize();
  }

  return true;
}
bool RenderColorVertex::Release() {
  m_Impl = nullptr;
  return true;
}

void RenderColorVertex::OnEnable() {}
void RenderColorVertex::OnDisable() {}

void RenderColorVertex::PreUpdate(const UpdateInfo& info) {
  auto& reg = m_World->GetReg();

  auto view = reg.view<AddComponent<Mesh<ColorVertex, uint32_t>>>();
  for (auto [e, addCom] : view.each()) {}
}
void RenderColorVertex::Update(const UpdateInfo& info) {}
void RenderColorVertex::PostUpdate(const UpdateInfo& info) {}