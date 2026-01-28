//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
// Developed by Minigraph
//
// Author:  James Stanard
//

#pragma once

#include "Quaternion.h"

namespace Math {
// Represents a 3x3 matrix while occuping a 3x4 memory footprint.  The unused row and column are undefined but implicitly
// (0, 0, 0, 1).  Constructing a Matrix4 will make those values explicit.
__declspec(align(16)) class Matrix3 {
 public:
  RE_FINLINE Matrix3() {}
  RE_FINLINE Matrix3(Vector3 x, Vector3 y, Vector3 z) {
    m_mat[0] = x;
    m_mat[1] = y;
    m_mat[2] = z;
  }
  RE_FINLINE Matrix3(const Matrix3& m) {
    m_mat[0] = m.m_mat[0];
    m_mat[1] = m.m_mat[1];
    m_mat[2] = m.m_mat[2];
  }
  RE_FINLINE Matrix3(Quaternion q) { *this = Matrix3(XMMatrixRotationQuaternion(q)); }
  RE_FINLINE explicit Matrix3(const XMMATRIX& m) {
    m_mat[0] = Vector3(m.r[0]);
    m_mat[1] = Vector3(m.r[1]);
    m_mat[2] = Vector3(m.r[2]);
  }
  RE_FINLINE explicit Matrix3(EIdentityTag) {
    m_mat[0] = Vector3(kXUnitVector);
    m_mat[1] = Vector3(kYUnitVector);
    m_mat[2] = Vector3(kZUnitVector);
  }
  RE_FINLINE explicit Matrix3(EZeroTag) { m_mat[0] = m_mat[1] = m_mat[2] = Vector3(kZero); }

  RE_FINLINE void SetX(Vector3 x) { m_mat[0] = x; }
  RE_FINLINE void SetY(Vector3 y) { m_mat[1] = y; }
  RE_FINLINE void SetZ(Vector3 z) { m_mat[2] = z; }

  RE_FINLINE Vector3 GetX() const { return m_mat[0]; }
  RE_FINLINE Vector3 GetY() const { return m_mat[1]; }
  RE_FINLINE Vector3 GetZ() const { return m_mat[2]; }

  static RE_FINLINE Matrix3 MakeXRotation(float angle) { return Matrix3(XMMatrixRotationX(angle)); }
  static RE_FINLINE Matrix3 MakeYRotation(float angle) { return Matrix3(XMMatrixRotationY(angle)); }
  static RE_FINLINE Matrix3 MakeZRotation(float angle) { return Matrix3(XMMatrixRotationZ(angle)); }
  static RE_FINLINE Matrix3 MakeScale(float scale) { return Matrix3(XMMatrixScaling(scale, scale, scale)); }
  static RE_FINLINE Matrix3 MakeScale(float sx, float sy, float sz) { return Matrix3(XMMatrixScaling(sx, sy, sz)); }
  static RE_FINLINE Matrix3 MakeScale(const XMFLOAT3& scale) { return Matrix3(XMMatrixScaling(scale.x, scale.y, scale.z)); }
  static RE_FINLINE Matrix3 MakeScale(Vector3 scale) { return Matrix3(XMMatrixScalingFromVector(scale)); }

  // Useful for DirectXMath interaction.  WARNING:  Only the 3x3 elements are defined.
  RE_FINLINE operator XMMATRIX() const { return XMMATRIX(m_mat[0], m_mat[1], m_mat[2], XMVectorZero()); }

  RE_FINLINE Matrix3 operator*(Scalar scl) const { return Matrix3(scl * GetX(), scl * GetY(), scl * GetZ()); }
  RE_FINLINE Vector3 operator*(Vector3 vec) const { return Vector3(XMVector3TransformNormal(vec, *this)); }
  RE_FINLINE Matrix3 operator*(const Matrix3& mat) const { return Matrix3(*this * mat.GetX(), *this * mat.GetY(), *this * mat.GetZ()); }

 private:
  Vector3 m_mat[3];
};

}  // namespace Math
