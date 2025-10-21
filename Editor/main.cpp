#include <REngine.h>

#include <QApplication>

#include "UI/MainWindow.h"

using namespace RE::Editor;

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  app.setApplicationName("Revolution-Editor");
  app.setApplicationVersion("1.0");
  app.setOrganizationName("Revolution");

  MainWindow window;
  window.show();

  return app.exec();
}