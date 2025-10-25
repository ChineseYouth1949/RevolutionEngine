#pragma once

#include <REngine/REngine.h>

#include <Setting/Setting.h>

namespace RE::Editor {
class MainWindow;

class REditor {
 public:
  REditor();
  ~REditor();

  void Initialize();

 private:
  void InitUI();

  static std::string sAppRunPath;
  static const std::string sMainConfigFileName;

  GUniquePtr<MainWindow> mMainWindow;
  GUniquePtr<Engine::Config> mMainConfig;
  GUniquePtr<Engine::Scene> mScene;
  GUniquePtr<Engine::IGraphicsCore> mGraphicsCore;
};

}  // namespace RE::Editor