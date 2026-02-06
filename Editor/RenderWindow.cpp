#include "RenderWindow.h"

#include <QCoreApplication>
#include <QKeyEvent>

namespace re::editor {
using namespace re::engine;
using namespace re::engine::render;
using namespace re::engine::ecs;

RenderWindow::RenderWindow(QWindow* parent) : QWindow(parent) {
  setFlags(flags() | Qt::FramelessWindowHint);
}
RenderWindow::~RenderWindow() {}

void RenderWindow::Init() {
  if (m_Init) {
    return;
  }

  // ECS scene init
  { m_Scene = GAlloc::make_shared<Scene>(); }

  // Render system
  {
    m_GC = GAlloc::make_shared<GraphicsCore>();

    QString appPathQt = QCoreApplication::applicationDirPath();
    std::wstring appPathStd = appPathQt.toStdWString();
    engine::wstring appPathRE = appPathStd.c_str();
    appPathRE = appPathRE + L"/Resource/Shader/";

    m_GC->SetResourcePath(appPathRE);

    GCInitInfo info;
    HWND hwnd = reinterpret_cast<HWND>(winId());
    info.swapChainHWND = hwnd;
    m_GC->Initialize(info);

    m_RenderCoreSystem = GAlloc::make_shared<CoreSystem>();
    m_RenderCoreSystem->Init(m_GC);

    m_Scene->AddSystem(m_RenderCoreSystem);
  }

  connect(&m_RunTimer, &QTimer::timeout, this, &RenderWindow::Update);

  m_RunTimer.start(1);

  m_Init = true;
}

void RenderWindow::Update() {
  m_Scene->Run();
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

void RenderWindow::keyPressEvent(QKeyEvent* event) {
  auto& camera = m_RenderCoreSystem->GetSharedInfo()->camera;
  m_RenderCoreSystem->GetSharedInfo()->Change();

  switch (event->key()) {
    case Qt::Key_W:
      camera.Walk(1);
      break;
    case Qt::Key_A:
      camera.Strafe(-1);
      break;
    case Qt::Key_S:
      camera.Walk(-1);
      break;
    case Qt::Key_D:
      camera.Strafe(1);
      break;
    default:
      QWindow::keyPressEvent(event);
  }

  camera.UpdateViewProj();
}
// void RenderWindow::keyReleaseEvent(QKeyEvent*) {}
// void RenderWindow::mousePressEvent(QMouseEvent*) {}
// void RenderWindow::mouseReleaseEvent(QMouseEvent*) {}
// void RenderWindow::mouseDoubleClickEvent(QMouseEvent*) {}
void RenderWindow::mouseMoveEvent(QMouseEvent* event) {
  auto& camera = m_RenderCoreSystem->GetSharedInfo()->camera;
  QPoint currentPos = event->pos();

  // 逻辑 1: 必须先检查按钮状态，再计算差值
  if (event->buttons() & Qt::MiddleButton) {
    m_RenderCoreSystem->GetSharedInfo()->Change();

    // 逻辑 2: 弧度转换。Win32 代码使用了 DirectX::XMConvertToRadians
    // 公式：radians = degrees * (PI / 180.0f)
    auto toRadians = [](float degrees) {
      return degrees * 0.0174532925f * 2.4;
    };

    // 逻辑 3: 严格对标 Win32 的计算公式 (dx * 0.25f 后再转弧度，最后除以 2)
    float dx = toRadians(0.25f * static_cast<float>(currentPos.x() - mMouseMidPressLastPos.x()));
    float dy = toRadians(0.25f * static_cast<float>(currentPos.y() - mMouseMidPressLastPos.y()));

    // 对标 Win32 的 dy / 2 和 dx / 2
    camera.Pitch(dy / 2.0f);
    camera.RotateY(dx / 2.0f);

    camera.UpdateViewProj();
  }

  // 逻辑 4: 无论是否按下，都要更新 LastPos (对应 Win32 的 mousePosx = LOWORD...)
  // 否则下次按下时 dx/dy 会因为巨大的跨度导致摄像机“瞬移”
  mMouseMidPressLastPos = currentPos;
}

}  // namespace re::editor