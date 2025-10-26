#pragma once

#include <QMainWindow>
#include <QBoxLayout>
#include <QMenuBar>
#include <QStatusBar>

#include "SceneWindow.h"

namespace RE::Editor {
class REditor;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow();
  ~MainWindow();

 protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseDoubleClickEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

  void wheelEvent(QWheelEvent* event) override;

  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void focusInEvent(QFocusEvent* event) override;
  void focusOutEvent(QFocusEvent* event) override;
  // void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;
  void moveEvent(QMoveEvent* event) override;
  void resizeEvent(QResizeEvent* event) override;
  void closeEvent(QCloseEvent* event) override;

  void showEvent(QShowEvent* event) override;
  void hideEvent(QHideEvent* event) override;

  // void paintEvent(QPaintEvent* event) override;
  // QPaintEngine* paintEngine() const override;

 private:
  void initUI();

  QWidget* mCentralwidget;
  QVBoxLayout* mVerticalLayout;
  QTabWidget* mTabWidget;

  QWidget* mSceneWidget;
  SceneWindow* mSceneWindow;

  QWidget* mGameTable;
  QMenuBar* mMenubar;
  QStatusBar* mStatusbar;

  friend class REditor;
};

}  // namespace RE::Editor