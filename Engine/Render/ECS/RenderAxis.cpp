#include "RenderAxis.h"

namespace re::engine::render {
using namespace Microsoft::WRL;

using namespace re::engine::ecs;
using namespace re::engine;

RenderAxis::RenderAxis() {}
RenderAxis::~RenderAxis() {}

void RenderAxis::Init(shared_ptr<GraphicsCore> gc, shared_ptr<SharedInfo> si) {
  m_GC = gc;
  m_SharedInfo = si;

  // RootSignature
  m_RootSignature.Reset(2, 0);
  m_RootSignature[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
  m_RootSignature[1].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_ALL);
  m_RootSignature.Finalize(L"MyRootSignature", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

  // INPUT_ELEMENT
  D3D12_INPUT_ELEMENT_DESC vertElem[] = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

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

  wstring vertexShaderPath = m_GC->GetResourceFilePath(L"InfiniteGrid.hlsl");
  RE_ASSERT_SUCCEEDED(D3DCompileFromFile(vertexShaderPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));

  wstring pixelShaderPath = m_GC->GetResourceFilePath(L"InfiniteGrid.hlsl");
  RE_ASSERT_SUCCEEDED(D3DCompileFromFile(pixelShaderPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

  m_PSO.SetVertexShader(vertexShader->GetBufferPointer(), vertexShader->GetBufferSize());
  m_PSO.SetPixelShader(pixelShader->GetBufferPointer(), pixelShader->GetBufferSize());

  // 6. 最终实例化
  m_PSO.Finalize();

  // VertexBuffer
  const float maxFloat = std::numeric_limits<float>::max();

  float halfMax = 1000;

  // Vertex triangleVertices[] = {{-halfMax, -halfMax, 0.0f}, {halfMax, -halfMax, 0.0f}, {-halfMax, halfMax, 0.0f},
  //                              {halfMax, -halfMax, 0.0f},  {halfMax, halfMax, 0.0f},  {-halfMax, halfMax, 0.0f}};
  //   mAspectRatio = static_cast<float>(mWidth) / static_cast<float>(mHeight);
  float mAspectRatio = static_cast<float>(m_SharedInfo->width) / static_cast<float>(m_SharedInfo->height);

  Vertex triangleVertices[] = {
      // triangle 1
      glm::vec3{0.0f, 0.25f * mAspectRatio, 0.0f}, glm::vec3{0.25f, -0.25f * mAspectRatio, 0.0f}, glm::vec3{-0.25f, -0.25f * mAspectRatio, 0.0f},
      // triangle 2
      glm::vec3{-halfMax, 0.0f, -halfMax}, glm::vec3{halfMax, 0.0f, -halfMax}, glm::vec3{halfMax, 0.0f, halfMax},
      // triangle 3
      glm::vec3{-halfMax, 0.0f, -halfMax}, glm::vec3{-halfMax, 0.0f, halfMax}, glm::vec3{halfMax, 0.0f, halfMax}};

  m_VertexBuffer.Create(L"VB_RenderAxis", 9, sizeof(Vertex), triangleVertices);

  // ConstBuffer
  memset(&m_ConstBuffer, 0, sizeof(m_ConstBuffer));
  m_ConstBuffer.backgroundColor = {0.24, 0.24, 0.24, 1.0f};
  m_ConstBuffer.gridColor = {0.63, 0.63, 0.63, 1.0};

  UpdateCB();
}

void RenderAxis::UpdateCB() {
  auto cameraPos = m_SharedInfo->camera.GetPosition();
  m_ConstBuffer.cameraPos = {cameraPos.x, cameraPos.y, cameraPos.z};
  m_ConstBuffer.screenResolution[0] = m_SharedInfo->width;
  m_ConstBuffer.screenResolution[1] = m_SharedInfo->height;
  m_ConstBuffer.gridSize = 1.0;
  m_ConstBuffer.lineWidth = 0.4;
  m_ConstBuffer.lineSoftness = 0.8;
}

void RenderAxis::Render() {
  auto pGfxContext = m_GC->GetGraphicsContext();

  pGfxContext->SetRootSignature(m_RootSignature);
  pGfxContext->SetPipelineState(m_PSO);
  pGfxContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  CameraCB cameraCB;
  cameraCB.view = m_SharedInfo->camera.GetViewMatrix();
  cameraCB.proj = m_SharedInfo->camera.GetProjMatrix();
  cameraCB.viewProj = m_SharedInfo->camera.GetViewProjMatrix();

  pGfxContext->SetDynamicConstantBufferView(0, sizeof(cameraCB), &cameraCB);
  pGfxContext->SetDynamicConstantBufferView(1, sizeof(m_ConstBuffer), &m_ConstBuffer);

  pGfxContext->SetVertexBuffer(0, m_VertexBuffer.VertexBufferView());
  pGfxContext->Draw(m_VertexBuffer.GetElementCount());
}

void RenderAxis::OnAttach(ecs::World* world) {
  System::OnAttach(world);
}
void RenderAxis::OnDetach() {
  System::OnDetach();
}

void RenderAxis::OnEnable() {}
void RenderAxis::OnDisable() {}

void RenderAxis::OnPreUpdate() {
  if (m_SharedInfo->Dirty()) {
    UpdateCB();
  }
}
void RenderAxis::OnPostUpdate() {
  Render();
}

}  // namespace re::engine::render