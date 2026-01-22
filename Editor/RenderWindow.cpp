#include "RenderWindow.h"

namespace re::editor {
RenderWindow::RenderWindow(QWindow* parent) : QWindow(parent) {
  setFlags(flags() | Qt::FramelessWindowHint);
}
RenderWindow::~RenderWindow() {}

// void RenderWindow::exposeEvent(QExposeEvent*) {}
// void RenderWindow::resizeEvent(QResizeEvent*) {}
// void RenderWindow::paintEvent(QPaintEvent*) {}
// void RenderWindow::moveEvent(QMoveEvent*) {}
// void RenderWindow::focusInEvent(QFocusEvent*) {}
// void RenderWindow::focusOutEvent(QFocusEvent*) {}

// void RenderWindow::showEvent(QShowEvent*) {}
// void RenderWindow::hideEvent(QHideEvent*) {}
// void RenderWindow::closeEvent(QCloseEvent*) {}

// void RenderWindow::keyPressEvent(QKeyEvent*) {}
// void RenderWindow::keyReleaseEvent(QKeyEvent*) {}
// void RenderWindow::mousePressEvent(QMouseEvent*) {}
// void RenderWindow::mouseReleaseEvent(QMouseEvent*) {}
// void RenderWindow::mouseDoubleClickEvent(QMouseEvent*) {}
// void RenderWindow::mouseMoveEvent(QMouseEvent*) {}

}  // namespace re::editor