// #pragma once

// #include "Graphics/Scene/ISceneImporter.h"

// #include <assimp/postprocess.h>
// #include <assimp/scene.h>
// #include <assimp/Importer.hpp>

// namespace RE::Core {

// class RE_API SceneImporterAssimp : public ISceneImporter {
//  public:
//   SceneImporterAssimp();
//   virtual ~SceneImporterAssimp();

//   SceneImporterAssimp(SceneImporterAssimp&&) noexcept;
//   SceneImporterAssimp& operator=(SceneImporterAssimp&&) noexcept;

//   SceneImporterAssimp(const SceneImporterAssimp&) = delete;
//   SceneImporterAssimp& operator=(const SceneImporterAssimp&) = delete;

//   bool LoadScene(std::string fileName, Flag64 flags);
//   std::vector<std::string> GetErrorString() const;

//   int GetSceneNum() const;
//   Scene* GetSceneIndex(int index);
//   std::vector<Scene*> GetAllScene();

//  protected:
//   void Destroy();

//  private:
//   Assimp::Importer* m_assimpImporter;
//   std::vector<Scene*> m_scenes;
//   std::vector<std::string> m_errorStrings;
// };

// }  // namespace RE::Core