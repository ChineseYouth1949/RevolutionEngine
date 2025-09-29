#pragma once

#include <memory>
#include <string>

#include "Base/Base.h"

#include "Scene.h"

namespace RE::Core {

class RE_API SceneImporter {
 public:
  SceneImporter();
  ~SceneImporter();

  bool IsUseable() const;

  bool LoadScene(const std::string& file, unsigned int flags);
  bool LoadSceneAsync(const std::string& file, unsigned int flags);

  std::shared_ptr<Scene> GetScene();
  void WaitAsyncComplete();

  bool LoadSuccess();
  std::string ErrorString() const;

 private:
  SceneImporter(SceneImporter&&) noexcept = default;
  SceneImporter& operator=(SceneImporter&&) noexcept = default;

  SceneImporter(const SceneImporter&) = delete;
  SceneImporter& operator=(const SceneImporter&) = delete;

  struct Impl;
  Impl* m_impl;

  static void LoadSceneToImporter(SceneImporter* pSceneImporter, const std::string& file, unsigned int flags, bool async);
};

}  // namespace RE::Core
