#pragma once

#include "Vector4f.h"

namespace RE::Engine {

class Matrix4x4 {
 public:
  Matrix4x4();

  struct {
    union {
      float value[4][4];
      Vector4f v[4];
    };
  };

  static Matrix4x4 Identity();

  Matrix4x4 operator*(const Matrix4x4& other) const;

  float& operator()(int row, int col);
  const float& operator()(int row, int col) const;

  Matrix4x4 transpose() const;
};

}  // namespace RE::Engine