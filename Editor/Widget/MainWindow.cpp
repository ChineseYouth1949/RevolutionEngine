#include "MainWindow.h"

namespace RE::Editor {

MainWindow::MainWindow() : QMainWindow() {
  initUI();

  resize(800, 600);
  setMinimumSize(QSize(700, 500));
}

MainWindow::~MainWindow() {}

void MainWindow::initUI() {
  mCentralwidget = new QWidget(this);
  mVerticalLayout = new QVBoxLayout(mCentralwidget);
  mTabWidget = new QTabWidget(mCentralwidget);

  mSceneWindow = new SceneWindow(nullptr);
  mSceneWidget = QWidget::createWindowContainer(mSceneWindow, mTabWidget);

  mTabWidget->addTab(mSceneWidget, QString("Scene"));
  mTabWidget->setTabText(0, QString("Scene"));
  mTabWidget->setCurrentIndex(0);

  mVerticalLayout->addWidget(mTabWidget);
  setCentralWidget(mCentralwidget);

  mMenubar = new QMenuBar(this);
  mMenubar->setObjectName("menubar");
  mMenubar->setGeometry(QRect(0, 0, 800, 25));
  setMenuBar(mMenubar);

  mStatusbar = new QStatusBar(this);
  mStatusbar->setObjectName("statusbar");
  setStatusBar(mStatusbar);

  setWindowTitle(QString("MainWindow"));
}

void MainWindow::mousePressEvent(QMouseEvent* event) {}
void MainWindow::mouseReleaseEvent(QMouseEvent* event) {}
void MainWindow::mouseDoubleClickEvent(QMouseEvent* event) {}
void MainWindow::mouseMoveEvent(QMouseEvent* event) {}

void MainWindow::wheelEvent(QWheelEvent* event) {}

void MainWindow::keyPressEvent(QKeyEvent* event) {}
void MainWindow::keyReleaseEvent(QKeyEvent* event) {}
void MainWindow::focusInEvent(QFocusEvent* event) {}
void MainWindow::focusOutEvent(QFocusEvent* event) {}
// void MainWindow::enterEvent(QEnterEvent* event) {}
void MainWindow::leaveEvent(QEvent* event) {}
void MainWindow::moveEvent(QMoveEvent* event) {}
void MainWindow::resizeEvent(QResizeEvent* event) {}
void MainWindow::closeEvent(QCloseEvent* event) {}

void MainWindow::showEvent(QShowEvent* event) {}
void MainWindow::hideEvent(QHideEvent* event) {}

void MainWindow::paintEvent(QPaintEvent* event) {}
QPaintEngine* MainWindow::paintEngine() const {
  return nullptr;
}

}  // namespace RE::Editor