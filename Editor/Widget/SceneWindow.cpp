#include "SceneWindow.h"

namespace RE::Editor {

SceneWindow::SceneWindow(QWindow* parent) : QWindow(parent) {
  setFlags(flags() | Qt::FramelessWindowHint);
}

void SceneWindow::exposeEvent(QExposeEvent*) {}
void SceneWindow::resizeEvent(QResizeEvent*) {}
void SceneWindow::paintEvent(QPaintEvent*) {}
void SceneWindow::moveEvent(QMoveEvent*) {}
void SceneWindow::focusInEvent(QFocusEvent*) {}
void SceneWindow::focusOutEvent(QFocusEvent*) {}

void SceneWindow::showEvent(QShowEvent*) {}
void SceneWindow::hideEvent(QHideEvent*) {}
void SceneWindow::closeEvent(QCloseEvent*) {}

// bool SceneWindow::event(QEvent*) {}
void SceneWindow::keyPressEvent(QKeyEvent*) {}
void SceneWindow::keyReleaseEvent(QKeyEvent*) {}
void SceneWindow::mousePressEvent(QMouseEvent*) {}
void SceneWindow::mouseReleaseEvent(QMouseEvent*) {}
void SceneWindow::mouseDoubleClickEvent(QMouseEvent*) {}
void SceneWindow::mouseMoveEvent(QMouseEvent*) {}

void SceneWindow::wheelEvent(QWheelEvent*) {}

}  // namespace RE::Editor