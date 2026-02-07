#pragma once

#include "QADS/DockWidget.h"
#include <QTreeWidget>

namespace re::editor {

class ProjectPanel : public QADS::DockWidget {
  Q_OBJECT
 public:
  explicit ProjectPanel(QWidget* parent = nullptr);
  ~ProjectPanel() override = default;

  void SetRootPath(const QString& path);
  void Refresh();

 private:
  QTreeWidget* m_tree{nullptr};
  QString m_root;
};

}  // namespace re::editor
