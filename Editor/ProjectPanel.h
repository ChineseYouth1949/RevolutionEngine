#pragma once

#include <QDockWidget>
#include <QTreeWidget>

namespace re::editor {

class ProjectPanel : public QDockWidget {
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
