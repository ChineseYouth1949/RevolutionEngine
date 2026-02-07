#include "SGWidget.h"
#include "RenderWindow.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QResizeEvent>

namespace re::editor {

SGWidget::SGWidget(const QString& title, QWidget* parent) : ads::CDockWidget(title, parent) {

  // 1. 创建主容器控件
  auto* mainWidget = new QWidget();
  // 核心：设置主容器的 SizePolicy 允许其向任何方向无限拉伸
  mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  auto* layout = new QVBoxLayout(mainWidget);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // 2. 初始化 TabWidget
  m_tabWidget = new QTabWidget();
  m_tabWidget->setDocumentMode(true);
  m_tabWidget->setTabsClosable(false);
  m_tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(m_tabWidget);
  setWidget(mainWidget);

  // --- Scene Tab 修复 ---
  auto* scenePage = new QWidget();
  auto* sceneLayout = new QVBoxLayout(scenePage);
  sceneLayout->setContentsMargins(0, 0, 0, 0);

  m_sceneWindow = new RenderWindow(nullptr);
  auto* sceneContainer = QWidget::createWindowContainer(m_sceneWindow);

  // 关键点：QWindowContainer 必须显式设置 Expanding 才能填满布局
  sceneContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  sceneLayout->addWidget(sceneContainer);
  m_tabWidget->addTab(scenePage, "Scene");

  // --- Game Tab 修复 ---
  auto* gamePage = new QWidget();
  auto* gameLayout = new QVBoxLayout(gamePage);
  gameLayout->setContentsMargins(0, 0, 0, 0);

  m_gameWindow = new RenderWindow(nullptr);
  auto* gameContainer = QWidget::createWindowContainer(m_gameWindow);
  gameContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  gameLayout->addWidget(gameContainer);
  m_tabWidget->addTab(gamePage, "Game");

  // 3. 强制同步事件
  connect(m_tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
    if (auto* current = m_tabWidget->widget(index)) {
      // 切换 Tab 时强制刷新当前页面的布局
      current->updateGeometry();
      if (current->layout()) {
        current->layout()->activate();
      }
    }
  });

  m_sceneWindow->Init();
}

// 3. 补充：重写 resizeEvent 确保 ADS 拖动时强制刷新
void SGWidget::resizeEvent(QResizeEvent* event) {
  ads::CDockWidget::resizeEvent(event);
  if (m_tabWidget) {
    // 在 Dock 窗口大小改变时，强制同步当前显示的子窗口
    m_tabWidget->setGeometry(this->rect());
  }
}

}  // namespace re::editor
