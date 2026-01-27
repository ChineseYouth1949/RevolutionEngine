#include <QApplication>

#include "mainwindow.h"

RE_EXPORT_D3D12_AGILITY_SDK

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  app.setApplicationName("REditor");
  app.setApplicationVersion("1.0");
  app.setOrganizationName("Revolution");

  re::editor::MainWindow window;
  window.resize(800, 600);
  window.show();

  return app.exec();
}