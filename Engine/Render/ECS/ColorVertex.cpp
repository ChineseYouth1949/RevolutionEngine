
#include "Engine/Core/Core.h"
#include "ColorVertex.h"

namespace re::engine::render {
using namespace Microsoft::WRL;

using namespace re::engine::ecs;
using namespace re::engine;

RenderColorVertex::RenderColorVertex() {}
RenderColorVertex::~RenderColorVertex() {}

void RenderColorVertex::Init(shared_ptr<GraphicsCore> gc) {
  m_GC = gc;

  // RootSignature
  m_RootSignature.Reset(2, 0);
  m_RootSignature[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
  m_RootSignature[1].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX);
  m_RootSignature.Finalize(L"MyRootSignature", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

  // INPUT_ELEMENT
  D3D12_INPUT_ELEMENT_DESC vertElem[] = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                                         {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

  // PSO
  GraphicsPSO m_PSO(L"MyColorPSO");

  // 1. 绑定之前创建好的根签名
  m_PSO.SetRootSignature(m_RootSignature);

  // 2. 设置渲染状态 (使用 MiniEngine 默认的常用状态)
  m_PSO.SetRasterizerState(Graphics::RasterizerDefault);      // 默认剔除和填充模式
  m_PSO.SetBlendState(Graphics::BlendDisable);                // 不开启混合
  m_PSO.SetDepthStencilState(Graphics::DepthStateReadWrite);  // 开启深度读写

  // 3. 配置输入布局和拓扑类型
  m_PSO.SetInputLayout(_countof(vertElem), vertElem);
  m_PSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

  // 4. 指定渲染目标格式 (需与你在应用中创建的 ColorBuffer 格式一致)
  // MiniEngine 默认主颜色缓冲区通常是 DXGI_FORMAT_R11G11B10_FLOAT 或 R8G8B8A8_UNORM
  m_PSO.SetRenderTargetFormat(Graphics::g_SceneColorBuffer.GetFormat(), Graphics::g_SceneDepthBuffer.GetFormat());

  // 5. 绑定着色器字节码
  // 这里的 g_pVS_Main 等通常由编译 .hlsl 生成的 .h 文件提供
  ComPtr<ID3DBlob> vertexShader;
  ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
  UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
  UINT compileFlags = 0;
#endif

  wstring vertexShaderPath = m_GC->GetResourceFilePath(L"HelloTriangle.hlsl");
  RE_ASSERT_SUCCEEDED(D3DCompileFromFile(vertexShaderPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));

  wstring pixelShaderPath = m_GC->GetResourceFilePath(L"HelloTriangle.hlsl");
  RE_ASSERT_SUCCEEDED(D3DCompileFromFile(pixelShaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

  m_PSO.SetVertexShader(vertexShader->GetBufferPointer(), vertexShader->GetBufferSize());
  m_PSO.SetPixelShader(pixelShader->GetBufferPointer(), pixelShader->GetBufferSize());

  // 6. 最终实例化
  m_PSO.Finalize();
}

RenderColorVertex::Resource RenderColorVertex::GetResource(RenderColorVertex::SysComType& com) {
  RenderColorVertex::Resource resource;

  resource.vertexBuffer.Create(L"ColorVertex1", com.vertexs.size(), SysComType::s_VertexSize, com.vertexs.data());
  resource.indexBuffer.Create(L"ColorVertex2", com.indexes.size(), SysComType::s_IndexSize, com.indexes.data());

  return resource;
}

void RenderColorVertex::PollAddCom() {
  auto& reg = m_World->GetRegistry();

  auto& stateStorage = reg.storage<StateComponentAdd<SysComType>>();
  for (auto [e, addCom] : stateStorage.each()) {
    auto resource = GetResource(addCom.data);
    m_EntityResrouce.insert({e, resource});
  }

  PollAddComponent<SysComType>();
}

void RenderColorVertex::PollDelCom() {
  auto& reg = m_World->GetRegistry();

  auto& stateStorage = reg.storage<StateComponentDel<SysComType>>();
  for (auto [e] : stateStorage.each()) {
    auto it = m_EntityResrouce.find(e);
    m_EntityResrouce.erase(it);
  }

  PollDelComponent<SysComType>();
}
void RenderColorVertex::PollChangeCom() {
  auto& reg = m_World->GetRegistry();

  auto& stateStorage = reg.storage<StateComponentDel<SysComType>>();
  for (auto [e] : stateStorage.each()) {
    auto it = m_EntityResrouce.find(e);
    auto& com = reg.get<SysComType>(e);
    it->second = GetResource(com);
  }
  PollChangeComponent<SysComType>();
}

void RenderColorVertex::Render() {
  auto pGfxContext = m_GC->GetGraphicsContext();

  pGfxContext->SetRootSignature(m_RootSignature);
  pGfxContext->SetPipelineState(m_PSO);
  pGfxContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  auto it = m_EntityResrouce.begin();
  while (it != m_EntityResrouce.end()) {
    pGfxContext->SetVertexBuffer(0, it->second.vertexBuffer.VertexBufferView());
    pGfxContext->SetIndexBuffer(it->second.indexBuffer.IndexBufferView());
    pGfxContext->DrawIndexed(it->second.indexBuffer.GetElementCount(), 0, 0);
    it++;
  }
}

bool RenderColorVertex::OnAttach() {
  return true;
}
bool RenderColorVertex::OnDetach() {
  return true;
}

void RenderColorVertex::OnEnable() {}
void RenderColorVertex::OnDisable() {}

void RenderColorVertex::OnPreUpdate(const ecs::UpdateInfo& info) {
  PollDelCom();
  PollChangeCom();
  PollAddCom();
}
void RenderColorVertex::OnUpdate(const ecs::UpdateInfo& info) {
  Render();
}
void RenderColorVertex::OnPostUpdate(const ecs::UpdateInfo& info) {}

}  // namespace re::engine::render