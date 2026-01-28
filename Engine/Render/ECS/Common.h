#pragma once

#include "Engine/Render/RHI/Math/Matrix4.h"
#include "Engine/Render/RHI/Camera.h"

namespace re::engine::render {
struct CameraCB {
  Math::Matrix4 view;
  Math::Matrix4 proj;
  Math::Matrix4 viewProj;
};

struct ModelCB {
  Math::Matrix4 model;
};

struct SharedInfo {
  Math::Camera camera;
};

}  // namespace re::engine::render