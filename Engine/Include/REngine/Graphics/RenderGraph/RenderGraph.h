#pragma once

#include "Base/CompileDefine.h"

#include "Resource/RenderResource.h"

#include <unordered_map>

namespace RE::Engine {

class RE_DECLSPEC RenderGraph {
 public:
  RenderGraph();
  ~RenderGraph();

  bool HaveResouce(RenderResource::Handle resHandle);
  bool AddResource(RenderResource* pResouce, bool load = false);
  RenderResource* GetResource(RenderResource::Handle resHandle);

  bool LoadResouce(RenderResource::Handle resHandle);
  bool LoadAllResouce();

  bool ReleaseResouce(RenderResource::Handle resHandle);
  bool ReleaseAllResouce();

 private:
  std::unordered_map<RenderResource::Handle, RenderResource*> mResouceMap;
};

}  // namespace RE::Engine