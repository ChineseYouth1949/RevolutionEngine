#include "Graphics/RenderGraph/RenderGraph.h"

namespace RE::Engine {

RenderGraph::RenderGraph() {}
RenderGraph::~RenderGraph() {}

bool RenderGraph::HaveResouce(RenderResource::Handle resHandle) {
  auto it = mResouceMap.find(resHandle);

  if (it == mResouceMap.end()) {
    return false;
  }

  if (it->second == nullptr) {
    return false;
  }

  return true;
}

bool RenderGraph::AddResource(RenderResource* pResouce, bool load) {
  bool haveRes = HaveResouce(pResouce->GetHandle());
  if (!haveRes) {
    mResouceMap.insert({pResouce->GetHandle(), pResouce});
  }

  bool success = true;
  if (load) {
    success = LoadResouce(pResouce->GetHandle());
  }

  return success;
}

RenderResource* RenderGraph::GetResource(RenderResource::Handle resHandle) {
  auto it = mResouceMap.find(resHandle);

  if (it == mResouceMap.end()) {
    return nullptr;
  }

  return it->second;
}

bool RenderGraph::RenderGraph::LoadResouce(RenderResource::Handle resHandle) {
  auto it = mResouceMap.find(resHandle);

  if (it == mResouceMap.end()) {
    return false;
  }

  if (it->second == nullptr) {
    return false;
  }

  auto pResouce = it->second;
  if (pResouce->IsLoad()) {
    return true;
  }

  return pResouce->Load();
}

bool RenderGraph::RenderGraph::LoadAllResouce() {
  auto it = mResouceMap.begin();

  bool allSuccess = true;
  while (it != mResouceMap.end()) {
    if (!LoadResouce(it->first)) {
      allSuccess = false;
    }

    it++;
  }

  return allSuccess;
}

bool RenderGraph::RenderGraph::ReleaseResouce(RenderResource::Handle resHandle) {
  auto it = mResouceMap.find(resHandle);

  if (it == mResouceMap.end()) {
    return false;
  }

  if (it->second == nullptr) {
    return false;
  }

  auto pResouce = it->second;
  if (!pResouce->IsLoad()) {
    return true;
  }

  bool success = pResouce->Release();
  if (success) {
    mResouceMap.erase(it);
  }

  return success;
}

}  // namespace RE::Engine
