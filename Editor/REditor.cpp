#include "REditor.h"

#include "Widget/MainWindow.h"

#include <QApplication>
#include <QTimer>

namespace RE::Editor {

const std::wstring REditor::sMainConfigFileName = L"Resource/Config/MainConfig.json";
std::wstring REditor::sAppRunPath = L"";

REditor::REditor()
    : mMainWindow(GMemoryAllocator::CreateUniquePtr<MainWindow>()),
      mMainConfig(GMemoryAllocator::CreateUniquePtr<Engine::Config>()),
      mGraphicsCore(GMemoryAllocator::Create<Engine::IGraphicsCore>()) {
  sAppRunPath = QApplication::applicationDirPath().toStdWString();
  sAppRunPath += L"/";
}

REditor::~REditor() {}

void REditor::Initialize() {
  auto windId = mMainWindow->winId();
  HWND windowHwnd = reinterpret_cast<HWND>(windId);

  Engine::GraphicsCoreConfig gcCofg;
  gcCofg.wHwnd = windowHwnd;
  gcCofg.wWdith = mMainWindow->width();
  gcCofg.wHeight = mMainWindow->height();
  gcCofg.appRunPath = sAppRunPath;
  gcCofg.resoucePath = (sAppRunPath + L"Resource/Graphics/");

  mGraphicsCore->Initialize(gcCofg);

  static QTimer* sRenderTimer = new QTimer();

  // 使用 lambda 表达式连接
  QObject::connect(sRenderTimer, &QTimer::timeout, [this] {
    if (mGraphicsCore) {
      mGraphicsCore->Render();
    }
  });

  sRenderTimer->start(16);

  InitUI();
}

void REditor::InitUI() {
  mMainWindow->showMaximized();
}

}  // namespace RE::Editor