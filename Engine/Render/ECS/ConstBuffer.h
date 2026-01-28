#pragma once

namespace re::engine::render {
struct CameraCB {
  Math::Matrix4 view;
  Math::Matrix4 proj;
  Math::Matrix4 viewProj;
};

struct ModelCB {
  Math::Matrix4 model;
};
}  // namespace re::engine::render