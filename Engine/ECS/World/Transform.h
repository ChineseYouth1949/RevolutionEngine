#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace re::engine::ecs {

struct Transform {
  glm::vec3 position{0.0f, 0.0f, 0.0f};
  glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f, 1.0f, 1.0f};

  glm::mat4 GetModelMatrix() const {
    // 1. 平移矩阵
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);

    // 2. 旋转矩阵 - 关键点：确保四元数已归一化，防止引入误差导致形变
    // 使用 glm::normalize(rotation) 确保它是标准的旋转
    glm::mat4 R = glm::mat4_cast(glm::normalize(rotation));

    // 3. 缩放矩阵
    glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

    // 4. 组合顺序：T * R * S (从右向左看：先缩放，再旋转，最后平移)
    return T * R * S;
  }
};

}  // namespace re::engine::ecs
