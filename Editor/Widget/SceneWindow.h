#pragma once

#include <QWindow>

namespace RE::Editor {

class SceneWindow : public QWindow {
  Q_OBJECT
 public:
  explicit SceneWindow(QWindow* parent);

 protected:
  void exposeEvent(QExposeEvent*) override;
  void resizeEvent(QResizeEvent*) override;
  void paintEvent(QPaintEvent*) override;
  void moveEvent(QMoveEvent*) override;
  void focusInEvent(QFocusEvent*) override;
  void focusOutEvent(QFocusEvent*) override;

  void showEvent(QShowEvent*) override;
  void hideEvent(QHideEvent*) override;
  void closeEvent(QCloseEvent*) override;

  //   bool event(QEvent*) override;
  void keyPressEvent(QKeyEvent*) override;
  void keyReleaseEvent(QKeyEvent*) override;
  void mousePressEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent*) override;
  void mouseDoubleClickEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent*) override;

  void wheelEvent(QWheelEvent*) override;
};

}  // namespace RE::Editor