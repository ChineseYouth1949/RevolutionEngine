#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace re::core::ecs {
struct Transform {
  glm::vec3 position{0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f};

  glm::mat4 worldMatrix{1.0f};

  bool dirty{true};
};

// struct Component {
//   template <class Component>
//   virtual void Serialize(flatbuffers::FlatBufferBuilder& fbb, Component com);
// };

// #include "Transform.flat.h"  // 确保这是你生成的文件名

// flatbuffers::Offset<re::core::ecs::Transform> SerializeTransform(flatbuffers::FlatBufferBuilder& fbb, const re::core::ecs::Transform& ecsData) {
//   // 1. 构建序列化所需的本地结构对象
//   // 注意：根据你的 fbs 定义，Vec3 和 Quat 是 struct 类型
//   re::core::ecs::Vec3 pos(ecsData.position.x, ecsData.position.y, ecsData.position.z);

//   // 注意：glm::quat 的构造或访问顺序通常是 [w, x, y, z]，
//   // 而你的 fbs 定义顺序是 [x, y, z, w]，请务必对应准确
//   re::core::ecs::Quat rot(ecsData.rotation.x, ecsData.rotation.y, ecsData.rotation.z, ecsData.rotation.w);

//   re::core::ecs::Vec3 scl(ecsData.scale.x, ecsData.scale.y, ecsData.scale.z);

//   // 2. 调用生成代码中的 Create 函数
//   // 该函数由 flatc 自动根据你的 table Transform 定义生成
//   return re::core::ecs::CreateTransform(fbb, &pos, &rot, &scl);
// }

}  // namespace re::core::ecs