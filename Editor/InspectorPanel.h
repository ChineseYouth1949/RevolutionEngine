#pragma once

#include "QADS/DockWidget.h"
#include <QWidget>
#include <QComboBox>
#include <QPushButton>

#include <Engine/All.h>

namespace re::editor {

class InspectorPanel : public ads::CDockWidget {
  Q_OBJECT
 public:
  explicit InspectorPanel(QWidget* parent = nullptr);
  ~InspectorPanel() override = default;

  void SetWorld(engine::ecs::World* world);
  void InspectEntity(engine::ecs::Entity entity);
  void ClearInspection();

 private:
  void BuildUI();
  void ShowTransform();
  void ShowMeshInfo();
  void OnAddComponentClicked();

  QWidget* m_topWidget{nullptr};
  QComboBox* m_compCombo{nullptr};
  QPushButton* m_addBtn{nullptr};

  engine::ecs::World* m_world{nullptr};
  engine::ecs::Entity m_entity{entt::null};
  QWidget* m_contents{nullptr};
};

}  // namespace re::editor
