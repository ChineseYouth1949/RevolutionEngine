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

#include "Matrix3.h"
#include "BoundingSphere.h"

namespace Math {
// Orthonormal basis (just rotation via quaternion) and translation
class OrthogonalTransform;

// A 3x4 matrix that allows for asymmetric skew and scale
class AffineTransform;

// Uniform scale and translation that can be compactly represented in a vec4
class ScaleAndTranslation;

// Uniform scale, rotation (quaternion), and translation that fits in two vec4s
class UniformTransform;

// This transform strictly prohibits non-uniform scale.  Scale itself is barely tolerated.
class OrthogonalTransform {
 public:
  RE_FINLINE OrthogonalTransform() : m_rotation(kIdentity), m_translation(kZero) {}
  RE_FINLINE OrthogonalTransform(Quaternion rotate) : m_rotation(rotate), m_translation(kZero) {}
  RE_FINLINE OrthogonalTransform(Vector3 translate) : m_rotation(kIdentity), m_translation(translate) {}
  RE_FINLINE OrthogonalTransform(Quaternion rotate, Vector3 translate) : m_rotation(rotate), m_translation(translate) {}
  RE_FINLINE OrthogonalTransform(const Matrix3& mat) : m_rotation(mat), m_translation(kZero) {}
  RE_FINLINE OrthogonalTransform(const Matrix3& mat, Vector3 translate) : m_rotation(mat), m_translation(translate) {}
  RE_FINLINE OrthogonalTransform(EIdentityTag) : m_rotation(kIdentity), m_translation(kZero) {}
  RE_FINLINE explicit OrthogonalTransform(const XMMATRIX& mat) { *this = OrthogonalTransform(Matrix3(mat), Vector3(mat.r[3])); }

  RE_FINLINE void SetRotation(Quaternion q) { m_rotation = q; }
  RE_FINLINE void SetTranslation(Vector3 v) { m_translation = v; }

  RE_FINLINE Quaternion GetRotation() const { return m_rotation; }
  RE_FINLINE Vector3 GetTranslation() const { return m_translation; }

  static RE_FINLINE OrthogonalTransform MakeXRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kXUnitVector), angle)); }
  static RE_FINLINE OrthogonalTransform MakeYRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kYUnitVector), angle)); }
  static RE_FINLINE OrthogonalTransform MakeZRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kZUnitVector), angle)); }
  static RE_FINLINE OrthogonalTransform MakeTranslation(Vector3 translate) { return OrthogonalTransform(translate); }

  RE_FINLINE Vector3 operator*(Vector3 vec) const { return m_rotation * vec + m_translation; }
  RE_FINLINE Vector4 operator*(Vector4 vec) const {
    return Vector4(SetWToZero(m_rotation * Vector3((XMVECTOR)vec))) + Vector4(SetWToOne(m_translation)) * vec.GetW();
  }
  RE_FINLINE BoundingSphere operator*(BoundingSphere sphere) const { return BoundingSphere(*this * sphere.GetCenter(), sphere.GetRadius()); }

  RE_FINLINE OrthogonalTransform operator*(const OrthogonalTransform& xform) const {
    return OrthogonalTransform(m_rotation * xform.m_rotation, m_rotation * xform.m_translation + m_translation);
  }

  RE_FINLINE OrthogonalTransform operator~() const {
    Quaternion invertedRotation = ~m_rotation;
    return OrthogonalTransform(invertedRotation, invertedRotation * -m_translation);
  }

 private:
  Quaternion m_rotation;
  Vector3 m_translation;
};

//
// A transform that lacks rotation and has only uniform scale.
//
class ScaleAndTranslation {
 public:
  RE_FINLINE ScaleAndTranslation() {}
  RE_FINLINE ScaleAndTranslation(EIdentityTag) : m_repr(kWUnitVector) {}
  RE_FINLINE ScaleAndTranslation(float tx, float ty, float tz, float scale) : m_repr(tx, ty, tz, scale) {}
  RE_FINLINE ScaleAndTranslation(Vector3 translate, Scalar scale) {
    m_repr = Vector4(translate);
    m_repr.SetW(scale);
  }
  RE_FINLINE explicit ScaleAndTranslation(const XMVECTOR& v) : m_repr(v) {}

  RE_FINLINE void SetScale(Scalar s) { m_repr.SetW(s); }
  RE_FINLINE void SetTranslation(Vector3 t) { m_repr.SetXYZ(t); }

  RE_FINLINE Scalar GetScale() const { return m_repr.GetW(); }
  RE_FINLINE Vector3 GetTranslation() const { return (Vector3)m_repr; }

  RE_FINLINE BoundingSphere operator*(const BoundingSphere& sphere) const {
    Vector4 scaledSphere = (Vector4)sphere * GetScale();
    Vector4 translation = Vector4(SetWToZero(m_repr));
    return BoundingSphere(scaledSphere + translation);
  }

 private:
  Vector4 m_repr;
};

//
// This transform allows for rotation, translation, and uniform scale
//
class UniformTransform {
 public:
  RE_FINLINE UniformTransform() {}
  RE_FINLINE UniformTransform(EIdentityTag) : m_rotation(kIdentity), m_translationScale(kIdentity) {}
  RE_FINLINE UniformTransform(Quaternion rotation, ScaleAndTranslation transScale) : m_rotation(rotation), m_translationScale(transScale) {}
  RE_FINLINE UniformTransform(Quaternion rotation, Scalar scale, Vector3 translation)
      : m_rotation(rotation), m_translationScale(translation, scale) {}

  RE_FINLINE void SetRotation(Quaternion r) { m_rotation = r; }
  RE_FINLINE void SetScale(Scalar s) { m_translationScale.SetScale(s); }
  RE_FINLINE void SetTranslation(Vector3 t) { m_translationScale.SetTranslation(t); }

  RE_FINLINE Quaternion GetRotation() const { return m_rotation; }
  RE_FINLINE Scalar GetScale() const { return m_translationScale.GetScale(); }
  RE_FINLINE Vector3 GetTranslation() const { return m_translationScale.GetTranslation(); }

  RE_FINLINE Vector3 operator*(Vector3 vec) const { return m_rotation * (vec * m_translationScale.GetScale()) + m_translationScale.GetTranslation(); }

  RE_FINLINE BoundingSphere operator*(BoundingSphere sphere) const {
    return BoundingSphere(*this * sphere.GetCenter(), GetScale() * sphere.GetRadius());
  }

 private:
  Quaternion m_rotation;
  ScaleAndTranslation m_translationScale;
};

// A AffineTransform is a 3x4 matrix with an implicit 4th row = [0,0,0,1].  This is used to perform a change of
// basis on 3D points.  An affine transformation does not have to have orthonormal basis vectors.
class AffineTransform {
 public:
  RE_FINLINE AffineTransform() {}
  RE_FINLINE AffineTransform(Vector3 x, Vector3 y, Vector3 z, Vector3 w) : m_basis(x, y, z), m_translation(w) {}
  RE_FINLINE AffineTransform(Vector3 translate) : m_basis(kIdentity), m_translation(translate) {}
  RE_FINLINE AffineTransform(const Matrix3& mat, Vector3 translate = Vector3(kZero)) : m_basis(mat), m_translation(translate) {}
  RE_FINLINE AffineTransform(Quaternion rot, Vector3 translate = Vector3(kZero)) : m_basis(rot), m_translation(translate) {}
  RE_FINLINE AffineTransform(const OrthogonalTransform& xform) : m_basis(xform.GetRotation()), m_translation(xform.GetTranslation()) {}
  RE_FINLINE AffineTransform(const UniformTransform& xform) {
    m_basis = Matrix3(xform.GetRotation()) * xform.GetScale();
    m_translation = xform.GetTranslation();
  }
  RE_FINLINE AffineTransform(EIdentityTag) : m_basis(kIdentity), m_translation(kZero) {}
  RE_FINLINE explicit AffineTransform(const XMMATRIX& mat) : m_basis(mat), m_translation(mat.r[3]) {}

  RE_FINLINE operator XMMATRIX() const { return (XMMATRIX&)*this; }

  RE_FINLINE void SetX(Vector3 x) { m_basis.SetX(x); }
  RE_FINLINE void SetY(Vector3 y) { m_basis.SetY(y); }
  RE_FINLINE void SetZ(Vector3 z) { m_basis.SetZ(z); }
  RE_FINLINE void SetTranslation(Vector3 w) { m_translation = w; }
  RE_FINLINE void SetBasis(const Matrix3& basis) { m_basis = basis; }

  RE_FINLINE Vector3 GetX() const { return m_basis.GetX(); }
  RE_FINLINE Vector3 GetY() const { return m_basis.GetY(); }
  RE_FINLINE Vector3 GetZ() const { return m_basis.GetZ(); }
  RE_FINLINE Vector3 GetTranslation() const { return m_translation; }
  RE_FINLINE const Matrix3& GetBasis() const { return (const Matrix3&)*this; }

  static RE_FINLINE AffineTransform MakeXRotation(float angle) { return AffineTransform(Matrix3::MakeXRotation(angle)); }
  static RE_FINLINE AffineTransform MakeYRotation(float angle) { return AffineTransform(Matrix3::MakeYRotation(angle)); }
  static RE_FINLINE AffineTransform MakeZRotation(float angle) { return AffineTransform(Matrix3::MakeZRotation(angle)); }
  static RE_FINLINE AffineTransform MakeScale(float scale) { return AffineTransform(Matrix3::MakeScale(scale)); }
  static RE_FINLINE AffineTransform MakeScale(Vector3 scale) { return AffineTransform(Matrix3::MakeScale(scale)); }
  static RE_FINLINE AffineTransform MakeTranslation(Vector3 translate) { return AffineTransform(translate); }

  RE_FINLINE Vector3 operator*(Vector3 vec) const { return m_basis * vec + m_translation; }
  RE_FINLINE AffineTransform operator*(const AffineTransform& mat) const {
    return AffineTransform(m_basis * mat.m_basis, *this * mat.GetTranslation());
  }

 private:
  Matrix3 m_basis;
  Vector3 m_translation;
};
}  // namespace Math
