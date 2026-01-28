#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace re::engine::render {

class RE_DECLSPEC Camera {
 public:
  Camera();
  ~Camera() = default;

  void SetPosition(const glm::vec3& pos);
  void SetFovY(float fovY);
  void SetLens(float fovY, float aspect, float zn, float zf);
  void LookAt(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up);

  void Strafe(float d);
  void Walk(float d);
  void Pitch(float angle);
  void RotateY(float angle);

  const glm::vec3& GetPosition() const;
  const glm::vec3& GetRight() const;
  const glm::vec3& GetUp() const;
  const glm::vec3& GetLook() const;

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

  glm::mat4 GetViewMatrix() const;
  glm::mat4 GetProjMatrix() const;
  glm::mat4 GetViewProjMatrix() const;

 private:
  glm::vec3 mPosition;
  glm::vec3 mRight;
  glm::vec3 mUp;
  glm::vec3 mLook;

  float mNearZ;
  float mFarZ;
  float mAspect;
  float mFovY;
  float mNearWindowHeight;
  float mFarWindowHeight;

  bool mViewDirty;

  glm::mat4 mView;
  glm::mat4 mProj;
};

}  // namespace re::engine::render
