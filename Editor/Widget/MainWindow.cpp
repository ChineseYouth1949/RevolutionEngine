#include <REngine.h>

#include "MainWindow.h"
#include "ui_mainwindow.h"

namespace RE::Editor {

struct MainWindow::Impl {
  RE::Engine::Scene mScene;
};

MainWindow::MainWindow() : QMainWindow(nullptr), mUi(new Ui::MainWindow), mImpl(nullptr) {
  mUi->setupUi(this);

  setMinimumSize(QSize(700, 500));

  showMaximized();
}
MainWindow::~MainWindow() {}

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