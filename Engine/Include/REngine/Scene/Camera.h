#pragma once

#include "Math/Math.h"

namespace RE::Engine {

class RE_DECLSPEC Camera {
 public:
  Camera();
  ~Camera() = default;

  void SetPosition(const Vector3f& pos);
  void SetFovY(float fovY);
  void SetLens(float fovY, float aspect, float zn, float zf);
  void LookAt(const Vector3f& pos, const Vector3f& target, const Vector3f& up);

  void Strafe(float d);
  void Walk(float d);
  void Pitch(float angle);
  void RotateY(float angle);

  const Vector3f& GetPosition() const;
  const Vector3f& GetRight() const;
  const Vector3f& GetUp() const;
  const Vector3f& GetLook() const;

  float GetNearZ() const;
  float GetFarZ() const;
  float GetAspect() const;
  float GetFovY() const;
  float GetFovX() const;

  float GetNearWindowWidth() const;
  float GetNearWindowHeight() const;
  float GetFarWindowWidth() const;
  float GetFarWindowHeight() const;

  void UpdateView();
  void UpdateProj();
  void UpdateViewProj();

  Matrix4x4 GetViewMatrix() const;
  Matrix4x4 GetProjMatrix() const;

 private:
  Vector3f mPosition;
  Vector3f mRight;
  Vector3f mUp;
  Vector3f mLook;

  float mNearZ;
  float mFarZ;
  float mAspect;
  float mFovY;
  float mNearWindowHeight;
  float mFarWindowHeight;

  bool mViewDirty;

  Matrix4x4 mView;
  Matrix4x4 mProj;
};

}  // namespace RE::Engine