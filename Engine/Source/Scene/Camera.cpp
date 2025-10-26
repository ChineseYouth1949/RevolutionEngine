#include "Scene/Camera.h"

#include <DirectXMath.h>

using namespace DirectX;

namespace RE::Engine {

Camera::Camera()
    : mPosition(0.0f, 0.0f, -10.0f),
      mRight(1.0f, 0.0f, 0.0f),
      mUp(0.0f, 1.0f, 0.0f),
      mLook(0.0f, 0.0f, 1.0f),
      mNearZ(0.1f),
      mFarZ(1000.0f),
      mAspect(1.0f),
      mFovY(XM_PIDIV4),  // 45度
      mNearWindowHeight(0.0f),
      mFarWindowHeight(0.0f),
      mViewDirty(true) {
  UpdateViewProj();
}

void Camera::SetPosition(const Vector3f& pos) {
  mPosition = pos;
  mViewDirty = true;
}

void Camera::SetFovY(float fovY) {
  mFovY = fovY;
  UpdateProj();
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf) {
  mFovY = fovY;
  mAspect = aspect;
  mNearZ = zn;
  mFarZ = zf;

  mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
  mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

  UpdateProj();
}

void Camera::LookAt(const Vector3f& pos, const Vector3f& target, const Vector3f& up) {
  mPosition = pos;

  XMVECTOR posVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&pos));
  XMVECTOR targetVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&target));
  XMVECTOR upVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&up));

  XMVECTOR lookVec = XMVector3Normalize(targetVec - posVec);
  XMVECTOR rightVec = XMVector3Normalize(XMVector3Cross(upVec, lookVec));
  XMVECTOR upVecNew = XMVector3Cross(lookVec, rightVec);

  XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mLook), lookVec);
  XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mRight), rightVec);
  XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mUp), upVecNew);

  mViewDirty = true;
}

void Camera::Strafe(float d) {
  mPosition.x += mRight.x * d;
  mPosition.y += mRight.y * d;
  mPosition.z += mRight.z * d;
  mViewDirty = true;
}

void Camera::Walk(float d) {
  mPosition.x += mLook.x * d;
  mPosition.y += mLook.y * d;
  mPosition.z += mLook.z * d;
  mViewDirty = true;
}

void Camera::Pitch(float angle) {
  XMMATRIX rotation = XMMatrixRotationAxis(XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&mRight)), angle);

  XMVECTOR upVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&mUp));
  XMVECTOR lookVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&mLook));

  upVec = XMVector3TransformNormal(upVec, rotation);
  lookVec = XMVector3TransformNormal(lookVec, rotation);

  XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mUp), upVec);
  XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mLook), lookVec);

  mViewDirty = true;
}

void Camera::RotateY(float angle) {
  XMMATRIX rotation = XMMatrixRotationY(angle);

  XMVECTOR rightVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&mRight));
  XMVECTOR upVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&mUp));
  XMVECTOR lookVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&mLook));

  rightVec = XMVector3TransformNormal(rightVec, rotation);
  upVec = XMVector3TransformNormal(upVec, rotation);
  lookVec = XMVector3TransformNormal(lookVec, rotation);

  XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mRight), rightVec);
  XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mUp), upVec);
  XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mLook), lookVec);

  mViewDirty = true;
}

const Vector3f& Camera::GetPosition() const {
  return mPosition;
}

const Vector3f& Camera::GetRight() const {
  return mRight;
}

const Vector3f& Camera::GetUp() const {
  return mUp;
}

const Vector3f& Camera::GetLook() const {
  return mLook;
}

float Camera::GetNearZ() const {
  return mNearZ;
}

float Camera::GetFarZ() const {
  return mFarZ;
}

float Camera::GetAspect() const {
  return mAspect;
}

float Camera::GetFovY() const {
  return mFovY;
}

float Camera::GetFovX() const {
  float halfWidth = 0.5f * GetNearWindowWidth();
  return 2.0f * atanf(halfWidth / mNearZ);
}

float Camera::GetNearWindowHeight() const {
  return mNearWindowHeight;
}

float Camera::GetFarWindowHeight() const {
  return mFarWindowHeight;
}

float Camera::GetNearWindowWidth() const {
  return mAspect * mNearWindowHeight;
}

float Camera::GetFarWindowWidth() const {
  return mAspect * mFarWindowHeight;
}

void Camera::UpdateView() {
  if (mViewDirty) {
    XMVECTOR posVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&mPosition));
    XMVECTOR rightVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&mRight));
    XMVECTOR upVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&mUp));
    XMVECTOR lookVec = XMLoadFloat3(reinterpret_cast<const XMFLOAT3*>(&mLook));

    // 正交化基向量
    lookVec = XMVector3Normalize(lookVec);
    upVec = XMVector3Normalize(XMVector3Cross(lookVec, rightVec));
    rightVec = XMVector3Cross(upVec, lookVec);

    // 重新存储正交化后的向量
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mRight), rightVec);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mUp), upVec);
    XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&mLook), lookVec);

    // 构建视图矩阵
    float x = -XMVectorGetX(XMVector3Dot(posVec, rightVec));
    float y = -XMVectorGetX(XMVector3Dot(posVec, upVec));
    float z = -XMVectorGetX(XMVector3Dot(posVec, lookVec));

    mView = Matrix4x4::Identity();
    mView(0, 0) = mRight.x;
    mView(0, 1) = mUp.x;
    mView(0, 2) = mLook.x;
    mView(0, 3) = 0.0f;
    mView(1, 0) = mRight.y;
    mView(1, 1) = mUp.y;
    mView(1, 2) = mLook.y;
    mView(1, 3) = 0.0f;
    mView(2, 0) = mRight.z;
    mView(2, 1) = mUp.z;
    mView(2, 2) = mLook.z;
    mView(2, 3) = 0.0f;
    mView(3, 0) = x;
    mView(3, 1) = y;
    mView(3, 2) = z;
    mView(3, 3) = 1.0f;

    mViewDirty = false;
  }
}

void Camera::UpdateProj() {
  XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);

  // 将DirectX矩阵转换到自定义矩阵格式
  XMFLOAT4X4 dxMatrix;
  XMStoreFloat4x4(&dxMatrix, projMatrix);

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      mProj(i, j) = dxMatrix.m[i][j];
    }
  }

  mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
  mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);
}

void Camera::UpdateViewProj() {
  UpdateView();
  UpdateProj();
}

Matrix4x4 Camera::GetViewMatrix() const {
  return mView;
}

Matrix4x4 Camera::GetProjMatrix() const {
  return mProj;
}

}  // namespace RE::Engine