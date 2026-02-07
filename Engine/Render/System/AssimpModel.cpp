#include "AssimpModel.h"

namespace re::engine::render {
using namespace Microsoft::WRL;
using namespace re::engine::ecs;
using namespace re::engine;

RenderAssimpModel::RenderAssimpModel() {}
RenderAssimpModel::~RenderAssimpModel() {}

void RenderAssimpModel::Init(shared_ptr<GraphicsCore> gc, shared_ptr<SharedInfo> si) {
  m_GC = gc;
  m_SharedInfo = si;

  // Root signature: 0 = CameraCB, 1 = ModelCB, 2 = MaterialCB
  m_RootSignature.Reset(3, 0);
  m_RootSignature[0].InitAsConstantBuffer(0, D3D12_SHADER_VISIBILITY_VERTEX);
  m_RootSignature[1].InitAsConstantBuffer(1, D3D12_SHADER_VISIBILITY_VERTEX);
  m_RootSignature[2].InitAsConstantBuffer(2, D3D12_SHADER_VISIBILITY_PIXEL);
  m_RootSignature.Finalize(L"RenderAssimpModel", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

  // Input layout: POSITION, NORMAL, TANGENT, TEXCOORD
  D3D12_INPUT_ELEMENT_DESC vertElem[] = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                                         {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                                         {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                                         {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};

  m_PSO.SetRootSignature(m_RootSignature);
  m_PSO.SetRasterizerState(Graphics::RasterizerDefault);
  m_PSO.SetBlendState(Graphics::BlendDisable);
  m_PSO.SetDepthStencilState(Graphics::DepthStateReadWrite);
  m_PSO.SetInputLayout(_countof(vertElem), vertElem);
  m_PSO.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
  m_PSO.SetRenderTargetFormat(Graphics::g_SceneColorBuffer.GetFormat(), Graphics::g_SceneDepthBuffer.GetFormat());

#if defined(_DEBUG)
  UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
  UINT compileFlags = 0;
#endif

  ComPtr<ID3DBlob> vertexShader;
  ComPtr<ID3DBlob> pixelShader;

  wstring vsPath = m_GC->GetResourceFilePath(L"PBRModel.hlsl");
  RE_ASSERT_SUCCEEDED(D3DCompileFromFile(vsPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
  RE_ASSERT_SUCCEEDED(D3DCompileFromFile(vsPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

  m_PSO.SetVertexShader(vertexShader->GetBufferPointer(), vertexShader->GetBufferSize());
  m_PSO.SetPixelShader(pixelShader->GetBufferPointer(), pixelShader->GetBufferSize());
  m_PSO.Finalize();
}

RenderAssimpModel::Resource RenderAssimpModel::GetResource(SysComType& com) {
  Resource resource;
  resource.vertexBuffer.Create(L"AssimpVB", com.vertexs.size(), SysComType::s_VertexSize, com.vertexs.data());
  resource.indexBuffer.Create(L"AssimpIB", com.indexes.size(), SysComType::s_IndexSize, com.indexes.data());
  return resource;
}

void RenderAssimpModel::PollAddComponent() {
  auto reg = m_World->GetRegistry();
  auto& stateStorage = reg->storage<AddComponentTag<SysComType>>();
  for (auto [e, addCom] : stateStorage.each()) {
    auto resource = GetResource(addCom.data);
    m_EntityResources.insert({e, resource});
  }
  PollAddComponentTemp<SysComType>();
}

void RenderAssimpModel::PollRemoveComponent() {
  auto reg = m_World->GetRegistry();
  auto& stateStorage = reg->storage<DelComponentTag<SysComType>>();
  for (auto [e] : stateStorage.each()) {
    auto it = m_EntityResources.find(e);
    if (it != m_EntityResources.end())
      m_EntityResources.erase(it);
  }
  PollDelComponentTemp<SysComType>();
}

void RenderAssimpModel::PollChangeComponent() {
  auto reg = m_World->GetRegistry();
  auto& stateStorage = reg->storage<ChangeComponentTag<SysComType>>();
  for (auto [e, com] : stateStorage.each()) {
    auto it = m_EntityResources.find(e);
    if (it != m_EntityResources.end())
      it->second = GetResource(com.data);
  }
  PollChangeComponentTemp<SysComType>();
}

void RenderAssimpModel::Render() {
  auto pGfxContext = m_GC->GetGraphicsContext();
  pGfxContext->SetRootSignature(m_RootSignature);
  pGfxContext->SetPipelineState(m_PSO);
  pGfxContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  CameraCB cameraCB;
  cameraCB.view = m_SharedInfo->camera.GetViewMatrix();
  cameraCB.proj = m_SharedInfo->camera.GetProjMatrix();
  cameraCB.viewProj = m_SharedInfo->camera.GetViewProjMatrix();
  pGfxContext->SetDynamicConstantBufferView(0, sizeof(cameraCB), &cameraCB);

  struct MaterialCB {
    glm::vec4 albedo;
    float metallic;
    float roughness;
    float pad;
  } materialCB;
  materialCB.albedo = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
  materialCB.metallic = 0.0f;
  materialCB.roughness = 0.5f;

  for (auto& [entity, resource] : m_EntityResources) {
    ModelCB modelCB;
    if (m_World->HasComponents<Transform>(entity)) {
      auto& t = m_World->GetComponent<Transform>(entity);
      modelCB.model = t.GetModelMatrix();
    } else {
      modelCB.model = glm::identity<glm::mat4>();
    }

    pGfxContext->SetDynamicConstantBufferView(1, sizeof(modelCB), &modelCB);
    pGfxContext->SetDynamicConstantBufferView(2, sizeof(materialCB), &materialCB);

    pGfxContext->SetVertexBuffer(0, resource.vertexBuffer.VertexBufferView());
    pGfxContext->SetIndexBuffer(resource.indexBuffer.IndexBufferView());
    pGfxContext->DrawIndexedInstanced(resource.indexBuffer.GetElementCount(), 1, 0, 0, 0);
  }
}

void RenderAssimpModel::OnAttach(ecs::World* world) {
  System::OnAttach(world);

  // Try to load a default model via Assimp if available, otherwise create a cube.
  SysComType com;
#ifdef RE_USE_ASSIMP
  wstring modelPath = m_GC->GetResourceFilePath(L"DefaultModel.obj");
  if (!LoadModelFromFile(modelPath, com)) {
    CreateDefaultCube(com);
  }
#else
  CreateDefaultCube(com);
#endif

  auto e = m_World->CreateEntity();
  m_World->AddComponentDelay<SysComType>(e, com);
}

void RenderAssimpModel::OnDetach() {
  System::OnDetach();
}

void RenderAssimpModel::OnEnable() {}
void RenderAssimpModel::OnDisable() {}

void RenderAssimpModel::OnPreUpdate() {
  PollRemoveComponent();
  PollChangeComponent();
  PollAddComponent();
}

void RenderAssimpModel::OnPostUpdate() {
  Render();
}

#ifdef RE_USE_ASSIMP
bool RenderAssimpModel::LoadModelFromFile(const wstring& path, SysComType& out) {
  string sPath;
  sPath.assign(path.begin(), path.end());
  Assimp::Importer importer;
  const aiScene* scene =
      importer.ReadFile(sPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded);
  if (!scene || !scene->HasMeshes())
    return false;

  // For simplicity load only first mesh
  aiMesh* mesh = scene->mMeshes[0];
  out.vertexs.clear();
  out.indexes.clear();
  out.vertexs.reserve(mesh->mNumVertices);
  for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
    PBRVertex v{};
    if (mesh->HasPositions())
      v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    if (mesh->HasNormals())
      v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
    if (mesh->HasTangents())
      v.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
    if (mesh->HasTextureCoords(0))
      v.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    out.vertexs.push_back(v);
  }

  for (unsigned f = 0; f < mesh->mNumFaces; ++f) {
    aiFace& face = mesh->mFaces[f];
    for (unsigned idx = 0; idx < face.mNumIndices; ++idx)
      out.indexes.push_back(face.mIndices[idx]);
  }

  return true;
}
#endif

void RenderAssimpModel::CreateDefaultCube(SysComType& out) {
  out.vertexs.clear();
  out.indexes.clear();

  array<PBRVertex, 8> vertices = {
      PBRVertex{glm::vec3(-1, -1, -1), glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec2(0, 0)},
      PBRVertex{glm::vec3(-1, 1, -1), glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec2(0, 1)},
      PBRVertex{glm::vec3(1, 1, -1), glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec2(1, 1)},
      PBRVertex{glm::vec3(1, -1, -1), glm::vec3(0, 0, -1), glm::vec3(1, 0, 0), glm::vec2(1, 0)},
      PBRVertex{glm::vec3(-1, -1, 1), glm::vec3(0, 0, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 0)},
      PBRVertex{glm::vec3(-1, 1, 1), glm::vec3(0, 0, 1), glm::vec3(-1, 0, 0), glm::vec2(0, 1)},
      PBRVertex{glm::vec3(1, 1, 1), glm::vec3(0, 0, 1), glm::vec3(-1, 0, 0), glm::vec2(1, 1)},
      PBRVertex{glm::vec3(1, -1, 1), glm::vec3(0, 0, 1), glm::vec3(-1, 0, 0), glm::vec2(1, 0)},
  };

  array<uint32_t, 36> indices = {0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 4, 1, 5, 4, 0, 1, 3, 6, 2, 3, 7, 6, 1, 2, 6, 1, 6, 5, 4, 3, 0, 4, 7, 3};

  for (auto& v : vertices)
    out.vertexs.push_back(v);
  for (auto& i : indices)
    out.indexes.push_back(i);
}

}  // namespace re::engine::render
