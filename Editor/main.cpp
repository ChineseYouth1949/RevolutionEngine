#include <QApplication>
#include <QMessageBox>

#include "mainwindow.h"
#include "RenderWindow.h"

RE_EXPORT_D3D12_AGILITY_SDK

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  app.setApplicationName("REditor");
  app.setApplicationVersion("1.0");
  app.setOrganizationName("Revolution");

  // #define TestSceneView 1

  try {
#if defined(TestSceneView)
    re::editor::RenderWindow renderWindow(nullptr);
    renderWindow.Init();
    renderWindow.showFullScreen();
#else
    re::editor::MainWindow mainWindow;
    // Prefer the larger default from MainWindow, but ensure initial size is generous
    mainWindow.resize(1600, 800);
    mainWindow.show();
#endif

    return app.exec();
  } catch (const std::system_error& e) {
    // 专门捕获 DX12 常见的系统错误
    QString msg = QString("System Error: %1\nCode: %2").arg(e.what()).arg(e.code().value());
    QMessageBox::critical(nullptr, "DX12 System Error", msg);
  } catch (const std::exception& e) {
    // 捕获其他标准异常
    QMessageBox::critical(nullptr, "C++ Exception", e.what());
  } catch (...) {
    // 捕获非标准异常（如信号、硬件异常等）
    QMessageBox::critical(nullptr, "Unknown Error", "An unspecified error occurred.");
  }
  return -1;
}