#include <QApplication>

#include "mainwindow.h"
#include "RenderWindow.h"

RE_EXPORT_D3D12_AGILITY_SDK

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  app.setApplicationName("REditor");
  app.setApplicationVersion("1.0");
  app.setOrganizationName("Revolution");

#define TestSceneView 1

#if defined(TestSceneView)
  re::editor::RenderWindow renderWindow(nullptr);
  renderWindow.Init();
  renderWindow.showFullScreen();
#else
  re::editor::MainWindow window;
  window.resize(800, 600);
  window.show();
#endif

  return app.exec();
}