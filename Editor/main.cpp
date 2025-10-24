#include "REditor.h"

#include <QApplication>

using namespace RE::Editor;

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  app.setApplicationName("REditor");
  app.setApplicationVersion("1.0");
  app.setOrganizationName("Revolution");

  REditor editor;
  editor.Initialize();

  return app.exec();
}