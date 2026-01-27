#include "SGWidget.h"
#include "RenderWindow.h"

#include <QDebug>

namespace re::editor {
SGWidget::SGWidget(const QString& title, QWidget* parent) : QDockWidget(title, parent) {
  // 1. 创建主容器和 Tab 控件
  auto* mainWidget = new QWidget();
  auto* layout = new QVBoxLayout(mainWidget);
  layout->setContentsMargins(0, 0, 0, 0);  // Unity 面板通常没有内边距

  m_tabWidget = new QTabWidget();
  m_tabWidget->setDocumentMode(true);  // 使 Tab 样式更扁平（类似现代编辑器）
  layout->addWidget(m_tabWidget);

  setWidget(mainWidget);

  // 2. 初始化 Scene 视图
  m_sceneWindow = new RenderWindow(nullptr);
  auto* sceneContainer = QWidget::createWindowContainer(m_sceneWindow);
  m_tabWidget->addTab(sceneContainer, "Scene");

  // 3. 初始化 Game 视图
  m_gameWindow = new RenderWindow(nullptr);
  auto* gameContainer = QWidget::createWindowContainer(m_gameWindow);
  m_tabWidget->addTab(gameContainer, "Game");

  // 4. 处理 Tab 切换事件
  connect(m_tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
    bool isScene = (index == 0);
    // 可以在此处暂停非活动视口的渲染以节省 GPU 资源
    qDebug() << "Active View:" << (isScene ? "Scene" : "Game");
  });

  m_sceneWindow->Init();
}
}  // namespace re::editor