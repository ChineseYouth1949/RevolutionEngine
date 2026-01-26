#pragma once

#include <QWindow>

namespace re::editor {

class RenderWindow : public QWindow {
  Q_OBJECT
 public:
  explicit RenderWindow(QWindow* parent);
  ~RenderWindow();

  void Initialize();
  void Release();

 protected:
  // void exposeEvent(QExposeEvent*) override;
  // void resizeEvent(QResizeEvent*) override;
  // void paintEvent(QPaintEvent*) override;
  // void moveEvent(QMoveEvent*) override;
  // void focusInEvent(QFocusEvent*) override;
  // void focusOutEvent(QFocusEvent*) override;

  // void showEvent(QShowEvent*) override;
  // void hideEvent(QHideEvent*) override;
  // void closeEvent(QCloseEvent*) override;

  // void keyPressEvent(QKeyEvent*) override;
  // void keyReleaseEvent(QKeyEvent*) override;
  // void mousePressEvent(QMouseEvent*) override;
  // void mouseReleaseEvent(QMouseEvent*) override;
  // void mouseDoubleClickEvent(QMouseEvent*) override;
  // void mouseMoveEvent(QMouseEvent*) override;
};

}  // namespace re::editor