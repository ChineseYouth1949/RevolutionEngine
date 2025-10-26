#include <QApplication>

#include "Widget/MainWindow.h"

using namespace RE::Editor;

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  app.setApplicationName("REditor");
  app.setApplicationVersion("1.0");
  app.setOrganizationName("Revolution");

  MainWindow window;
  window.show();

  return app.exec();
}