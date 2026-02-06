#include <QApplication>

#include "mainwindow.h"
#include "RenderWindow.h"

RE_EXPORT_D3D12_AGILITY_SDK

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

  app.setApplicationName("REditor");
  app.setApplicationVersion("1.0");
  app.setOrganizationName("Revolution");

  // re::editor::MainWindow window;
  // window.resize(800, 600);
  // window.show();

  re::editor::RenderWindow renderWindow(nullptr);
  renderWindow.Init();
  renderWindow.showFullScreen();

  return app.exec();
}