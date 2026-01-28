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

    m_RenderSystem = GAlloc::make_shared<RenderSystem>();
    m_RenderSystem->Init(m_GC);

    m_Scene->AddSystem(m_RenderSystem);
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
  auto& camera = m_RenderSystem->GetSharedInfo()->camera;
  m_RenderSystem->GetSharedInfo()->Change();

  // 定义移动速度
  const float moveSpeed = 1.0f;

  // 获取相机当前的朝向轴
  // MiniEngine 约定：-Z 是 Forward，+X 是 Right
  Math::Vector3 forward = camera.GetForwardVec();
  Math::Vector3 right = camera.GetRightVec();
  Math::Vector3 currentPos = camera.GetPosition();

  switch (event->key()) {
    case Qt::Key_W:
      // 前进：位置 + 前向向量
      camera.SetPosition(currentPos + forward * moveSpeed);
      break;
    case Qt::Key_S:
      // 后退：位置 - 前向向量
      camera.SetPosition(currentPos - forward * moveSpeed);
      break;
    case Qt::Key_A:
      // 左横移：位置 - 右向向量
      camera.SetPosition(currentPos - right * moveSpeed);
      break;
    case Qt::Key_D:
      // 右横移：位置 + 右向向量
      camera.SetPosition(currentPos + right * moveSpeed);
      break;
    default:
      QWindow::keyPressEvent(event);
      return;  // 没按移动键则直接返回，不执行下面的 Update
  }

  // 重要：修改位置后必须同步更新矩阵，否则渲染器无法感知相机移动
  camera.Update();
}
// void RenderWindow::keyReleaseEvent(QKeyEvent*) {}
// void RenderWindow::mousePressEvent(QMouseEvent*) {}
// void RenderWindow::mouseReleaseEvent(QMouseEvent*) {}
// void RenderWindow::mouseDoubleClickEvent(QMouseEvent*) {}
void RenderWindow::mouseMoveEvent(QMouseEvent* event) {
  auto& camera = m_RenderSystem->GetSharedInfo()->camera;
  m_RenderSystem->GetSharedInfo()->Change();

  if (event->buttons() & Qt::MiddleButton) {
    QPoint currentPos = event->pos();

    // 1. 计算鼠标位移（单位：像素）
    float dx = static_cast<float>(currentPos.x() - mMouseMidPressLastPos.x());
    float dy = static_cast<float>(currentPos.y() - mMouseMidPressLastPos.y());

    // 2. 定义平移灵敏度（建议根据距离或 Fov 调整）
    float sensitivity = 0.05f;

    // 3. 获取摄像机当前的坐标系轴向 (Basis)
    // MiniEngine 中：GetRightVec 为 +X, GetUpVec 为 +Y
    Math::Vector3 right = camera.GetRightVec();
    Math::Vector3 up = camera.GetUpVec();

    // 4. 计算世界空间下的位移矢量
    // 鼠标向右移动(dx > 0) -> 摄像机向左平移 -> 减去 Right 向量
    // 鼠标向下移动(dy > 0) -> 摄像机向上平移 -> 加上 Up 向量
    Math::Vector3 offset = (right * -dx + up * dy) * sensitivity;

    // 5. 更新位置
    camera.SetPosition(camera.GetPosition() + offset);

    // 6. 必须调用 Update 重新生成矩阵
    camera.Update();

    mMouseMidPressLastPos = currentPos;
  }
}

}  // namespace re::editor