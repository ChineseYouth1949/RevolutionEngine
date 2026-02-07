#include "ColorVertex.h"

namespace re::engine::render {
using namespace Microsoft::WRL;

using namespace re::engine::ecs;
using namespace re::engine;

RenderColorVertex::RenderColorVertex() {}
RenderColorVertex::~RenderColorVertex() {}

void RenderColorVertex::Init(shared_ptr<GraphicsCore> gc, shared_ptr<SharedInfo> si) {
  m_GC = gc;
  m_SharedInfo = si;

  // RootSignature
  m_RootSignature.Reset(2, 0);
  m_RootSignature[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
  m_RootSignature[1].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX);
  m_RootSignature.Finalize(L"RenderColorVertex", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

  // INPUT_ELEMENT
  D3D12_INPUT_ELEMENT_DESC vertElem[] = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                                         {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

  // PSO
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

  wstring vertexShaderPath = m_GC->GetResourceFilePath(L"ColorVertex.hlsl");
  RE_ASSERT_SUCCEEDED(D3DCompileFromFile(vertexShaderPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));

  wstring pixelShaderPath = m_GC->GetResourceFilePath(L"ColorVertex.hlsl");
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

void RenderColorVertex::PollAddComponent() {
  auto reg = m_World->GetRegistry();

  auto& stateStorage = reg->storage<AddComponentTag<SysComType>>();
  for (auto [e, addCom] : stateStorage.each()) {
    auto resource = GetResource(addCom.data);
    m_EntityResources.insert({e, resource});
  }

  PollAddComponentTemp<SysComType>();
}

void RenderColorVertex::PollRemoveComponent() {
  auto reg = m_World->GetRegistry();

  auto& stateStorage = reg->storage<DelComponentTag<SysComType>>();
  for (auto [e] : stateStorage.each()) {
    auto it = m_EntityResources.find(e);
    if (it != m_EntityResources.end()) {
      m_EntityResources.erase(it);
    }
  }

  PollDelComponentTemp<SysComType>();
}
void RenderColorVertex::PollChangeComponent() {
  auto reg = m_World->GetRegistry();

  auto& stateStorage = reg->storage<ChangeComponentTag<SysComType>>();
  for (auto [e, com] : stateStorage.each()) {
    auto it = m_EntityResources.find(e);
    if (it != m_EntityResources.end()) {
      it->second = GetResource(com.data);
    }
  }
  PollChangeComponentTemp<SysComType>();
}

void RenderColorVertex::Render() {
  auto pGfxContext = m_GC->GetGraphicsContext();

  pGfxContext->SetRootSignature(m_RootSignature);
  pGfxContext->SetPipelineState(m_PSO);
  pGfxContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  CameraCB cameraCB;
  cameraCB.view = m_SharedInfo->camera.GetViewMatrix();
  cameraCB.proj = m_SharedInfo->camera.GetProjMatrix();
  cameraCB.viewProj = m_SharedInfo->camera.GetViewProjMatrix();

  pGfxContext->SetDynamicConstantBufferView(0, sizeof(cameraCB), &cameraCB);

  // For each entity we render, set model CB from its Transform component
  for (auto& [entity, resource] : m_EntityResources) {
    ModelCB modelCB;
    if (m_World->HasComponents<Transform>(entity)) {
      auto& t = m_World->GetComponent<Transform>(entity);
      // Convert glm::mat4 to caller's expected type (glm used here)
      modelCB.model = t.GetModelMatrix();
    } else {
      modelCB.model = glm::identity<glm::mat4>();
    }

    pGfxContext->SetDynamicConstantBufferView(1, sizeof(modelCB), &modelCB);

    pGfxContext->SetVertexBuffer(0, resource.vertexBuffer.VertexBufferView());
    pGfxContext->SetIndexBuffer(resource.indexBuffer.IndexBufferView());
    pGfxContext->DrawIndexedInstanced(resource.indexBuffer.GetElementCount(), 1, 0, 0, 0);
  }
}

void RenderColorVertex::OnAttach(ecs::World* world) {
  System::OnAttach(world);

  array<ColorVertex, 8> vertices = {
      ColorVertex{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 0.7f)},  // White
      ColorVertex{glm::vec3(-1.0f, +1.0f, -1.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.7f)},  // Black
      ColorVertex{glm::vec3(+1.0f, +1.0f, -1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 0.7f)},  // Red
      ColorVertex{glm::vec3(+1.0f, -1.0f, -1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.7f)},  // Green
      ColorVertex{glm::vec3(-1.0f, -1.0f, +1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 0.7f)},  // Blue
      ColorVertex{glm::vec3(-1.0f, +1.0f, +1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 0.7f)},  // Yellow
      ColorVertex{glm::vec3(+1.0f, +1.0f, +1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 0.7f)},  // Cyan
      ColorVertex{glm::vec3(+1.0f, -1.0f, +1.0f), glm::vec4(1.0f, 0.0f, 1.0f, 0.7f)}   // Magenta
  };

  m_DefaultVertexBuffer.Create(L"DefaultColorVertexVB", vertices.size(), sizeof(ColorVertex), vertices.data());

  array<uint32_t, 36> indices = {// 前 (0, 1, 2 -> 0, 2, 1)
                                 0, 2, 1, 0, 3, 2,
                                 // 后 (4, 6, 5 -> 4, 5, 6)
                                 4, 5, 6, 4, 6, 7,
                                 // 左
                                 4, 1, 5, 4, 0, 1,
                                 // 右
                                 3, 6, 2, 3, 7, 6,
                                 // 上
                                 1, 6, 5, 1, 2, 6,
                                 // 下
                                 4, 3, 0, 4, 7, 3};

  // test data
  SysComType com;

  com.vertexs.reserve(vertices.size());
  for (auto vertex : vertices) {
    com.vertexs.push_back(vertex);
  }

  com.indexes.reserve(indices.size());
  for (const auto& index : indices) {
    com.indexes.push_back(index);
  }

  auto e = m_World->CreateEntity();
  m_World->AddComponentDelay<SysComType>(e, com);
}
void RenderColorVertex::OnDetach() {
  System::OnDetach();
}

void RenderColorVertex::OnEnable() {}
void RenderColorVertex::OnDisable() {}

void RenderColorVertex::OnPreUpdate() {
  PollRemoveComponent();
  PollChangeComponent();
  PollAddComponent();

  // for (auto& [e, sysCom] : m_World->GetRegistry()->view<SysComType>().each()) {
  //   auto newCom = sysCom;
  //   for (auto& vertex : newCom.vertexs) {
  //     vertex.position += glm::vec3(0.01f, 0.01f, 0.001f);
  //   }
  //   m_World->ChangeComponentDelay<SysComType>(e, newCom);
  // }

  for (auto& [e, sysCom] : m_World->GetRegistry()->view<Transform>().each()) {
    sysCom.position += glm::vec3(0.01f, 0.01f, 0.001f);
  }
}
void RenderColorVertex::OnPostUpdate() {
  Render();
}

}  // namespace re::engine::render