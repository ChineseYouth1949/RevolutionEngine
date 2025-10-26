#include "REditor.h"

#include "SceneWindow.h"
#include <QKeyEvent>

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
void SceneWindow::keyPressEvent(QKeyEvent* event) {
  auto pEditor = REditor::Instance();
  auto pGC = pEditor->GetGraphicsCore();
  auto pCamera = pGC->GetCamera();

  switch (event->key()) {
    case Qt::Key_W:
      pCamera->Walk(1);
      break;
    case Qt::Key_A:
      pCamera->Strafe(-1);
      break;
    case Qt::Key_S:
      pCamera->Walk(-1);
      break;
    case Qt::Key_D:
      pCamera->Strafe(1);
      break;
    default:
      QWindow::keyPressEvent(event);
  }
}

void SceneWindow::keyReleaseEvent(QKeyEvent*) {}
void SceneWindow::mousePressEvent(QMouseEvent*) {}
void SceneWindow::mouseReleaseEvent(QMouseEvent*) {}
void SceneWindow::mouseDoubleClickEvent(QMouseEvent*) {}
void SceneWindow::mouseMoveEvent(QMouseEvent*) {}

void SceneWindow::wheelEvent(QWheelEvent*) {}

}  // namespace RE::Editor