#pragma once

#include <QWindow>
#include <QTimer>
#include <QPoint>

#include <Engine/All.h>

namespace re::editor {
class RenderWindow : public QWindow {
  Q_OBJECT
 public:
  explicit RenderWindow(QWindow* parent);
  ~RenderWindow();

  void Init();

 protected:
  void Update();
  void OnResize(int width, int height);

  // void exposeEvent(QExposeEvent*) override;
  void resizeEvent(QResizeEvent*) override;
  // void paintEvent(QPaintEvent*) override;
  // void moveEvent(QMoveEvent*) override;
  // void focusInEvent(QFocusEvent*) override;
  // void focusOutEvent(QFocusEvent*) override;

  // void showEvent(QShowEvent*) override;
  // void hideEvent(QHideEvent*) override;
  // void closeEvent(QCloseEvent*) override;

  void keyPressEvent(QKeyEvent*) override;
  // void keyReleaseEvent(QKeyEvent*) override;
  // void mousePressEvent(QMouseEvent*) override;
  // void mouseReleaseEvent(QMouseEvent*) override;
  // void mouseDoubleClickEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent*) override;

  bool m_Init{false};
  engine::shared_ptr<engine::ecs::Scene> m_Scene;
  engine::shared_ptr<engine::render::GraphicsCore> m_GC;
  engine::shared_ptr<engine::render::CoreSystem> m_RenderCoreSystem;

  QTimer m_RunTimer;

  QPoint mMouseMidPressLastPos;
};

}  // namespace re::editor