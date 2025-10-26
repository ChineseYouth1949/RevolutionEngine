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
void SceneWindow::focusOutEvent(QFocusEvent* event) {
  setCursor(Qt::ArrowCursor);
}

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
void SceneWindow::mousePressEvent(QMouseEvent* event) {
  if (event->button() == Qt::MiddleButton) {
    mMouseMidPressStartPos = event->pos();
    mMouseMidPressLastPos = event->pos();
    setCursor(Qt::ClosedHandCursor);
  }
}
void SceneWindow::mouseReleaseEvent(QMouseEvent* event) {
  if (event->button() == Qt::MiddleButton) {
    setCursor(Qt::ArrowCursor);
  }
}
void SceneWindow::mouseDoubleClickEvent(QMouseEvent*) {}
void SceneWindow::mouseMoveEvent(QMouseEvent* event) {
  auto pEditor = REditor::Instance();
  auto pGC = pEditor->GetGraphicsCore();
  auto pCamera = pGC->GetCamera();

  if (event->buttons() & Qt::MiddleButton) {
    QPoint currentPos = event->pos();

    float dx = qDegreesToRadians(0.25f * (currentPos.x() - mMouseMidPressLastPos.x()));
    float dy = qDegreesToRadians(0.25f * (currentPos.y() - mMouseMidPressLastPos.y()));

    pCamera->Pitch(dy / 2.0f);
    pCamera->RotateY(dx / 2.0f);

    mMouseMidPressLastPos = currentPos;
  }
}

void SceneWindow::wheelEvent(QWheelEvent* event) {
  auto pEditor = REditor::Instance();
  auto pGC = pEditor->GetGraphicsCore();
  auto pCamera = pGC->GetCamera();

  int delta = event->angleDelta().y();
  float currentFovY = pCamera->GetFovY();
  float newFovY = currentFovY - delta / 1200.0f / 5;

  pCamera->SetFovY(newFovY);

  event->accept();
}

}  // namespace RE::Editor