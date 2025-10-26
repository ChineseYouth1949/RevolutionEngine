#pragma once

#include <REngine/REngine.h>
#include <Setting/Setting.h>

namespace RE::Editor {
class MainWindow;

class REditor {
 public:
  static void BuildInstance(MainWindow* mainWindow);
  static REditor* Instance();

  Engine::IGraphicsCore* GetGraphicsCore();

 private:
  REditor(MainWindow* mainWindow);
  ~REditor();

  void Initialize();

  static inline REditor* sInstance = nullptr;
  static std::wstring sAppRunPath;
  static const std::wstring sMainConfigFileName;

  MainWindow* mMainWindow;
  GUniquePtr<Engine::Config> mMainConfig;
  GUniquePtr<Engine::IGraphicsCore> mGraphicsCore;
  GUniquePtr<Engine::Scene> mScene;
};

}  // namespace RE::Editor