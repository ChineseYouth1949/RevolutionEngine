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

void REditor::Initialize() {
  bool success = mMainConfig->LoadFromFile(sAppRunPath + sMainConfigFileName);
  REAssert(success);

  InitUI();
}

void REditor::InitUI() {
  bool success = mMainConfig->HasOption("window_min_size");
  REAssert(success);

  // auto& value = mMainConfig->GetOption("window_min_size");

  mMainWindow->showMaximized();
}

}  // namespace RE::Editor