#include "REditor.h"

#include "Widget/MainWindow.h"

#include <QApplication>

namespace RE::Editor {

const std::string REditor::sMainConfigFileName = "Resource/Config/MainConfig.json";
std::string REditor::sAppRunPath = "";

REditor::REditor() : mMainWindow(GMemoryAllocator::CreateUniquePtr<MainWindow>()), mMainConfig(GMemoryAllocator::CreateUniquePtr<Engine::Config>()) {
  sAppRunPath = QApplication::applicationDirPath().toStdString();
  sAppRunPath += "/";
}

REditor::~REditor() {}

Engine::Result REditor::Initialize() {
  Engine::Result result;

  bool success = mMainConfig->LoadFromFile(sAppRunPath + sMainConfigFileName);

  mMainWindow->showMaximized();

  return result;
}

}  // namespace RE::Editor