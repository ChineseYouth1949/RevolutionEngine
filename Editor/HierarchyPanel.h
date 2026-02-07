#pragma once

#include "QADS/DockWidget.h"
#include <QListWidget>
#include <QMenu>
#include <Engine/All.h>

namespace re::editor {

class InspectorPanel;

class HierarchyPanel : public ads::CDockWidget {
  Q_OBJECT
 public:
  explicit HierarchyPanel(QWidget* parent = nullptr);
  ~HierarchyPanel() override = default;

  void SetWorld(engine::ecs::World* world);
  void Refresh();
  void SetInspector(InspectorPanel* inspector) { m_inspector = inspector; }

 private slots:
  void OnItemClicked(QListWidgetItem* item);
  void OnContextRequested(const QPoint& pos);
  void OnCreateEntity();
  void OnDeleteEntity();

 private:
  void RefreshEntityList();

  QListWidget* m_list{nullptr};
  QMenu* m_menu{nullptr};
  engine::ecs::World* m_world{nullptr};
  InspectorPanel* m_inspector{nullptr};
  engine::ecs::Entity m_selected{entt::null};
};

}  // namespace re::editor
