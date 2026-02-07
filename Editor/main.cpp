#include <QApplication>

#include "mainwindow.h"
#include "RenderWindow.h"

#include "QADS/DockManager.h"
#include "QADS/DockWidget.h"

RE_EXPORT_D3D12_AGILITY_SDK

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  app.setApplicationName("REditor");
  app.setApplicationVersion("1.0");
  app.setOrganizationName("Revolution");

  // #define TestSceneView 1

#if defined(TestSceneView)
  re::editor::RenderWindow renderWindow(nullptr);
  renderWindow.Init();
  renderWindow.showFullScreen();
#else
  re::editor::MainWindow mainWindow;
  mainWindow.resize(800, 600);
  mainWindow.show();

  ads::CDockManager* m_DockManager = new ads::CDockManager(&mainWindow);
#endif

  return app.exec();
}