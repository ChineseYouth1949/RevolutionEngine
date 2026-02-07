#include "ProjectPanel.h"

#include <QVBoxLayout>
#include <QDir>
#include <QTreeWidgetItem>
#include <QDockWidget>

namespace re::editor {

ProjectPanel::ProjectPanel(QWidget* parent) : ads::CDockWidget("Project", parent) {
  m_tree = new QTreeWidget(this);
  m_tree->setHeaderHidden(true);
  setWidget(m_tree);

  // Allow bottom docking and movable/floating if underlying widget supports QDockWidget API
  if (auto dock = qobject_cast<QDockWidget*>(this)) {
    dock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
  }
}

void ProjectPanel::SetRootPath(const QString& path) {
  m_root = path;
  Refresh();
}

void ProjectPanel::Refresh() {
  m_tree->clear();
  if (m_root.isEmpty())
    return;
  QDir dir(m_root);
  auto list = dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  for (auto& name : list) {
    QTreeWidgetItem* item = new QTreeWidgetItem(m_tree);
    item->setText(0, name);
    m_tree->addTopLevelItem(item);
  }
}

}  // namespace re::editor
