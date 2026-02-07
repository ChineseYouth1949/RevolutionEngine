#include "SGWidget.h"
#include "RenderWindow.h"

#include <QDebug>
#include <QDockWidget>

namespace re::editor {
SGWidget::SGWidget(const QString& title, QWidget* parent) : ads::CDockWidget(title, parent) {
  auto* mainWidget = new QWidget();
  auto* layout = new QVBoxLayout(mainWidget);
  layout->setContentsMargins(0, 0, 0, 0);

  m_tabWidget = new QTabWidget();
  m_tabWidget->setDocumentMode(true);
  m_tabWidget->setTabsClosable(false);
  layout->addWidget(m_tabWidget);

  setWidget(mainWidget);

  if (auto dock = qobject_cast<QDockWidget*>(this)) {
    dock->setAllowedAreas(Qt::AllDockWidgetAreas);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
  }

  m_sceneWindow = new RenderWindow(nullptr);
  auto* sceneContainer = QWidget::createWindowContainer(m_sceneWindow);
  m_tabWidget->addTab(sceneContainer, "Scene");

  m_gameWindow = new RenderWindow(nullptr);
  auto* gameContainer = QWidget::createWindowContainer(m_gameWindow);
  m_tabWidget->addTab(gameContainer, "Game");

  connect(m_tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
    bool isScene = (index == 0);
    qDebug() << "Active View:" << (isScene ? "Scene" : "Game");
  });

  m_sceneWindow->Init();
}
}  // namespace re::editor