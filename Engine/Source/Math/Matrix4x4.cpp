
#include "Math/Matrix4x4.h"

#include <memory>

namespace RE::Engine {

Matrix4x4::Matrix4x4() {
  memset(value, 0, sizeof(value));
}

Matrix4x4 Matrix4x4::Identity() {
  Matrix4x4 result;
  // 设置对角线为1
  for (int i = 0; i < 4; ++i) {
    result.value[i][i] = 1.0f;
  }
  return result;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const {
  Matrix4x4 result;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result.value[i][j] = 0;
      for (int k = 0; k < 4; ++k) {
        result.value[i][j] += value[i][k] * other.value[k][j];
      }
    }
  }
  return result;
}

float& Matrix4x4::operator()(int row, int col) {
  return value[row][col];
}

const float& Matrix4x4::operator()(int row, int col) const {
  return value[row][col];
}

Matrix4x4 Matrix4x4::transpose() const {
  Matrix4x4 result;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      result.value[j][i] = value[i][j];
    }
  }
  return result;
}

}  // namespace RE::Engine