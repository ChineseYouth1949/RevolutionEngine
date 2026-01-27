#include "RenderWindow.h"

namespace re::editor {
using namespace re::engine;
using namespace re::engine::render;
using namespace re::engine::ecs;

RenderWindow::RenderWindow(QWindow* parent) : QWindow(parent) {
  setFlags(flags() | Qt::FramelessWindowHint);
}
RenderWindow::~RenderWindow() {}

void RenderWindow::Initialize() {
  if (m_Init) {
    return;
  }

  // ECS scene init
  { m_Scene = GAlloc::make_shared<Scene>(); }

  // render system
  {
    m_GC = GAlloc::make_shared<GraphicsCore>();

    GCInitInfo info;
    m_GC->Initialize(info);

    m_RenderSystem = GAlloc::make_shared<RenderSystem>();
    m_RenderSystem->Init(m_GC);

    m_Scene->AddSystem(m_RenderSystem);
  }

  m_Init = true;
}

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