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

#include "Scalar.h"

namespace Math {
class Vector4;

// A 3-vector with an unspecified fourth component.  Depending on the context, the W can be 0 or 1, but both are implicit.
// The actual value of the fourth component is undefined for performance reasons.
class Vector3 {
 public:
  RE_FINLINE Vector3() {}
  RE_FINLINE Vector3(float x, float y, float z) { m_vec = XMVectorSet(x, y, z, z); }
  RE_FINLINE Vector3(const XMFLOAT3& v) { m_vec = XMLoadFloat3(&v); }
  RE_FINLINE Vector3(const Vector3& v) { m_vec = v; }
  RE_FINLINE Vector3(Scalar s) { m_vec = s; }
  RE_FINLINE explicit Vector3(Vector4 vec);
  RE_FINLINE explicit Vector3(FXMVECTOR vec) { m_vec = vec; }
  RE_FINLINE explicit Vector3(EZeroTag) { m_vec = SplatZero(); }
  RE_FINLINE explicit Vector3(EIdentityTag) { m_vec = SplatOne(); }
  RE_FINLINE explicit Vector3(EXUnitVector) { m_vec = CreateXUnitVector(); }
  RE_FINLINE explicit Vector3(EYUnitVector) { m_vec = CreateYUnitVector(); }
  RE_FINLINE explicit Vector3(EZUnitVector) { m_vec = CreateZUnitVector(); }

  RE_FINLINE operator XMVECTOR() const { return m_vec; }

  RE_FINLINE Scalar GetX() const { return Scalar(XMVectorSplatX(m_vec)); }
  RE_FINLINE Scalar GetY() const { return Scalar(XMVectorSplatY(m_vec)); }
  RE_FINLINE Scalar GetZ() const { return Scalar(XMVectorSplatZ(m_vec)); }
  RE_FINLINE void SetX(Scalar x) { m_vec = XMVectorPermute<4, 1, 2, 3>(m_vec, x); }
  RE_FINLINE void SetY(Scalar y) { m_vec = XMVectorPermute<0, 5, 2, 3>(m_vec, y); }
  RE_FINLINE void SetZ(Scalar z) { m_vec = XMVectorPermute<0, 1, 6, 3>(m_vec, z); }

  RE_FINLINE Vector3 operator-() const { return Vector3(XMVectorNegate(m_vec)); }
  RE_FINLINE Vector3 operator+(Vector3 v2) const { return Vector3(XMVectorAdd(m_vec, v2)); }
  RE_FINLINE Vector3 operator-(Vector3 v2) const { return Vector3(XMVectorSubtract(m_vec, v2)); }
  RE_FINLINE Vector3 operator*(Vector3 v2) const { return Vector3(XMVectorMultiply(m_vec, v2)); }
  RE_FINLINE Vector3 operator/(Vector3 v2) const { return Vector3(XMVectorDivide(m_vec, v2)); }
  RE_FINLINE Vector3 operator*(Scalar v2) const { return *this * Vector3(v2); }
  RE_FINLINE Vector3 operator/(Scalar v2) const { return *this / Vector3(v2); }
  RE_FINLINE Vector3 operator*(float v2) const { return *this * Scalar(v2); }
  RE_FINLINE Vector3 operator/(float v2) const { return *this / Scalar(v2); }

  RE_FINLINE Vector3& operator+=(Vector3 v) {
    *this = *this + v;
    return *this;
  }
  RE_FINLINE Vector3& operator-=(Vector3 v) {
    *this = *this - v;
    return *this;
  }
  RE_FINLINE Vector3& operator*=(Vector3 v) {
    *this = *this * v;
    return *this;
  }
  RE_FINLINE Vector3& operator/=(Vector3 v) {
    *this = *this / v;
    return *this;
  }

  RE_FINLINE friend Vector3 operator*(Scalar v1, Vector3 v2) { return Vector3(v1) * v2; }
  RE_FINLINE friend Vector3 operator/(Scalar v1, Vector3 v2) { return Vector3(v1) / v2; }
  RE_FINLINE friend Vector3 operator*(float v1, Vector3 v2) { return Scalar(v1) * v2; }
  RE_FINLINE friend Vector3 operator/(float v1, Vector3 v2) { return Scalar(v1) / v2; }

 protected:
  XMVECTOR m_vec;
};

// A 4-vector, completely defined.
class Vector4 {
 public:
  RE_FINLINE Vector4() {}
  RE_FINLINE Vector4(float x, float y, float z, float w) { m_vec = XMVectorSet(x, y, z, w); }
  RE_FINLINE Vector4(const XMFLOAT4& v) { m_vec = XMLoadFloat4(&v); }
  RE_FINLINE Vector4(Vector3 xyz, float w) { m_vec = XMVectorSetW(xyz, w); }
  RE_FINLINE Vector4(const Vector4& v) { m_vec = v; }
  RE_FINLINE Vector4(const Scalar& s) { m_vec = s; }
  RE_FINLINE explicit Vector4(Vector3 xyz) { m_vec = SetWToOne(xyz); }
  RE_FINLINE explicit Vector4(FXMVECTOR vec) { m_vec = vec; }
  RE_FINLINE explicit Vector4(EZeroTag) { m_vec = SplatZero(); }
  RE_FINLINE explicit Vector4(EIdentityTag) { m_vec = SplatOne(); }
  RE_FINLINE explicit Vector4(EXUnitVector) { m_vec = CreateXUnitVector(); }
  RE_FINLINE explicit Vector4(EYUnitVector) { m_vec = CreateYUnitVector(); }
  RE_FINLINE explicit Vector4(EZUnitVector) { m_vec = CreateZUnitVector(); }
  RE_FINLINE explicit Vector4(EWUnitVector) { m_vec = CreateWUnitVector(); }

  RE_FINLINE operator XMVECTOR() const { return m_vec; }

  RE_FINLINE Scalar GetX() const { return Scalar(XMVectorSplatX(m_vec)); }
  RE_FINLINE Scalar GetY() const { return Scalar(XMVectorSplatY(m_vec)); }
  RE_FINLINE Scalar GetZ() const { return Scalar(XMVectorSplatZ(m_vec)); }
  RE_FINLINE Scalar GetW() const { return Scalar(XMVectorSplatW(m_vec)); }
  RE_FINLINE void SetX(Scalar x) { m_vec = XMVectorPermute<4, 1, 2, 3>(m_vec, x); }
  RE_FINLINE void SetY(Scalar y) { m_vec = XMVectorPermute<0, 5, 2, 3>(m_vec, y); }
  RE_FINLINE void SetZ(Scalar z) { m_vec = XMVectorPermute<0, 1, 6, 3>(m_vec, z); }
  RE_FINLINE void SetW(Scalar w) { m_vec = XMVectorPermute<0, 1, 2, 7>(m_vec, w); }
  RE_FINLINE void SetXYZ(Vector3 xyz) { m_vec = XMVectorPermute<0, 1, 2, 7>(xyz, m_vec); }

  RE_FINLINE Vector4 operator-() const { return Vector4(XMVectorNegate(m_vec)); }
  RE_FINLINE Vector4 operator+(Vector4 v2) const { return Vector4(XMVectorAdd(m_vec, v2)); }
  RE_FINLINE Vector4 operator-(Vector4 v2) const { return Vector4(XMVectorSubtract(m_vec, v2)); }
  RE_FINLINE Vector4 operator*(Vector4 v2) const { return Vector4(XMVectorMultiply(m_vec, v2)); }
  RE_FINLINE Vector4 operator/(Vector4 v2) const { return Vector4(XMVectorDivide(m_vec, v2)); }
  RE_FINLINE Vector4 operator*(Scalar v2) const { return *this * Vector4(v2); }
  RE_FINLINE Vector4 operator/(Scalar v2) const { return *this / Vector4(v2); }
  RE_FINLINE Vector4 operator*(float v2) const { return *this * Scalar(v2); }
  RE_FINLINE Vector4 operator/(float v2) const { return *this / Scalar(v2); }

  RE_FINLINE void operator*=(float v2) { *this = *this * Scalar(v2); }
  RE_FINLINE void operator/=(float v2) { *this = *this / Scalar(v2); }

  RE_FINLINE friend Vector4 operator*(Scalar v1, Vector4 v2) { return Vector4(v1) * v2; }
  RE_FINLINE friend Vector4 operator/(Scalar v1, Vector4 v2) { return Vector4(v1) / v2; }
  RE_FINLINE friend Vector4 operator*(float v1, Vector4 v2) { return Scalar(v1) * v2; }
  RE_FINLINE friend Vector4 operator/(float v1, Vector4 v2) { return Scalar(v1) / v2; }

 protected:
  XMVECTOR m_vec;
};

// Defined after Vector4 methods are declared
RE_FINLINE Vector3::Vector3(Vector4 vec) : m_vec((XMVECTOR)vec) {}

// For W != 1, divide XYZ by W.  If W == 0, do nothing
RE_FINLINE Vector3 MakeHomogeneous(Vector4 v) {
  Scalar W = v.GetW();
  return Vector3(XMVectorSelect(XMVectorDivide(v, W), v, XMVectorEqual(W, SplatZero())));
}

class BoolVector {
 public:
  RE_FINLINE BoolVector(FXMVECTOR vec) { m_vec = vec; }
  RE_FINLINE operator XMVECTOR() const { return m_vec; }

 protected:
  XMVECTOR m_vec;
};

}  // namespace Math
