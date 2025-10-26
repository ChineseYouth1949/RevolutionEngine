#include "REditor.h"

#include "Widget/MainWindow.h"

#include <QApplication>
#include <QTimer>

namespace RE::Editor {

const std::wstring REditor::sMainConfigFileName = L"Resource/Config/MainConfig.json";
std::wstring REditor::sAppRunPath = L"";

void REditor::BuildInstance(MainWindow* mainWindow) {
  if (!sInstance) {
    sInstance = new REditor(mainWindow);
  }
}

REditor* REditor::Instance() {
  REAssert(sInstance);
  return sInstance;
}

REditor::REditor(MainWindow* mainWindow)
    : mMainWindow(mainWindow),
      mMainConfig(GMemoryAllocator::CreateUniquePtr<Engine::Config>()),
      mGraphicsCore(GMemoryAllocator::Create<Engine::IGraphicsCore>()) {
  sAppRunPath = QApplication::applicationDirPath().toStdWString();
  sAppRunPath += L"/";

  Initialize();
}

REditor::~REditor() {}

void REditor::Initialize() {
  auto windId = mMainWindow->mSceneWindow->winId();
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
      mGraphicsCore->Update();
      mGraphicsCore->Render();
    }
  });

  sRenderTimer->start(16);
}

Engine::IGraphicsCore* REditor::GetGraphicsCore() {
  return mGraphicsCore.get();
}

}  // namespace RE::Editor