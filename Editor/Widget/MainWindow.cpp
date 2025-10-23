#include <REngine.h>

#include "MainWindow.h"
#include "ui_mainwindow.h"

using namespace RE::Engine;

namespace RE::Editor {

struct MainWindow::Impl {
  MainWindow::Impl() : mScene(GAllocateConstructor<Scene>()), mGraphicsCore(GAllocateConstructor<GraphicsCore>()) {}

  GUniquePtr<Scene> mScene;
  GUniquePtr<GraphicsCore> mGraphicsCore;
};

MainWindow::MainWindow() : QMainWindow(nullptr), mUi(new Ui::MainWindow), mImpl(nullptr) {
  mImpl = new Impl();

  mUi->setupUi(this);

  setMinimumSize(QSize(700, 500));

  showMaximized();
}

MainWindow::~MainWindow() {
  delete mImpl;
  mImpl = nullptr;
}

void MainWindow::Initialize() {}

void MainWindow::mousePressEvent(QMouseEvent* event) {}
void MainWindow::mouseReleaseEvent(QMouseEvent* event) {}
void MainWindow::mouseDoubleClickEvent(QMouseEvent* event) {}
void MainWindow::mouseMoveEvent(QMouseEvent* event) {}

void MainWindow::wheelEvent(QWheelEvent* event) {}

void MainWindow::keyPressEvent(QKeyEvent* event) {}
void MainWindow::keyReleaseEvent(QKeyEvent* event) {}
void MainWindow::focusInEvent(QFocusEvent* event) {}
void MainWindow::focusOutEvent(QFocusEvent* event) {}
void MainWindow::enterEvent(QEnterEvent* event) {}
void MainWindow::leaveEvent(QEvent* event) {}
void MainWindow::paintEvent(QPaintEvent* event) {}
void MainWindow::moveEvent(QMoveEvent* event) {}
void MainWindow::resizeEvent(QResizeEvent* event) {}
void MainWindow::closeEvent(QCloseEvent* event) {}

void MainWindow::showEvent(QShowEvent* event) {}
void MainWindow::hideEvent(QHideEvent* event) {}

}  // namespace RE::Editor