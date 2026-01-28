#pragma once

#include "Engine/Render/RHI/Math/Matrix4.h"
#include "Engine/Render/RHI/Camera.h"

#include "Camera.h"

namespace re::engine::render {
struct CameraCB {
  glm::mat4 view;
  glm::mat4 proj;
  glm::mat4 viewProj;
};

struct ModelCB {
  glm::mat4 model;
};

struct SharedInfo {
  Camera camera;
  int width;
  int height;

  void Change() { dirty = true; }
  bool Dirty() { return dirty; }
  void Reset() { dirty = false; }

 private:
  bool dirty = true;
};

}  // namespace re::engine::render