#include "InspectorPanel.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QComboBox>

#include "Engine/Render/System/ColorVertex.h"

namespace re::editor {

InspectorPanel::InspectorPanel(QWidget* parent) : QDockWidget("Inspector", parent) {
  setObjectName("InspectorPanel");
  BuildUI();
}

void InspectorPanel::SetWorld(engine::ecs::World* world) {
  m_world = world;
}

void InspectorPanel::BuildUI() {
  m_contents = new QWidget(this);
  auto* layout = new QVBoxLayout(m_contents);
  layout->setContentsMargins(4, 4, 4, 4);

  // Top controls for adding components
  m_topWidget = new QWidget(m_contents);
  auto* topLay = new QHBoxLayout(m_topWidget);
  m_compCombo = new QComboBox(m_topWidget);
  m_compCombo->addItem("Transform");
  m_compCombo->addItem("ColorMesh");
  m_addBtn = new QPushButton("Add Component", m_topWidget);
  topLay->addWidget(m_compCombo);
  topLay->addWidget(m_addBtn);
  layout->addWidget(m_topWidget);

  setWidget(m_contents);

  connect(m_addBtn, &QPushButton::clicked, this, &InspectorPanel::OnAddComponentClicked);
}

void InspectorPanel::InspectEntity(engine::ecs::Entity entity) {
  m_entity = entity;

  // Clear previous
  delete m_contents;
  BuildUI();

  // Show Transform if present
  if (m_world && m_world->HasComponents<engine::ecs::Transform>(m_entity)) {
    ShowTransform();
  }

  // Show Mesh info if present (RenderColorVertex::SysComType)
  using SysComType = re::engine::render::RenderColorVertex::SysComType;
  if (m_world && m_world->HasComponents<SysComType>(m_entity)) {
    ShowMeshInfo();
  }
}

void InspectorPanel::ClearInspection() {
  m_entity = entt::null;
  delete m_contents;
  BuildUI();
}

void InspectorPanel::ShowTransform() {
  if (!m_world)
    return;
  auto& t = m_world->GetComponent<engine::ecs::Transform>(m_entity);

  auto* group = new QGroupBox("Transform", m_contents);
  auto* form = new QFormLayout(group);
  form->addRow("Position", new QLabel(QString::fromStdString(std::to_string(t.position.x) + ", " + std::to_string(t.position.y) + ", " +
                                                             std::to_string(t.position.z))));
  form->addRow("Scale",
               new QLabel(QString::fromStdString(std::to_string(t.scale.x) + ", " + std::to_string(t.scale.y) + ", " + std::to_string(t.scale.z))));
  m_contents->layout()->addWidget(group);
}

void InspectorPanel::ShowMeshInfo() {
  using SysComType = re::engine::render::RenderColorVertex::SysComType;
  auto& mesh = m_world->GetComponent<SysComType>(m_entity);

  auto* group = new QGroupBox("Mesh (ColorVertex)", m_contents);
  auto* form = new QFormLayout(group);
  form->addRow("VertexCount", new QLabel(QString::number(mesh.vertexs.size())));
  form->addRow("IndexCount", new QLabel(QString::number(mesh.indexes.size())));
  m_contents->layout()->addWidget(group);
}

void InspectorPanel::OnAddComponentClicked() {
  if (!m_world || m_entity == entt::null)
    return;
  QString type = m_compCombo->currentText();
  if (type == "Transform") {
    // Add default transform
    engine::ecs::Transform t;
    m_world->AddComponent<engine::ecs::Transform>(m_entity, t);
  } else if (type == "ColorMesh") {
    using SysComType = re::engine::render::RenderColorVertex::SysComType;
    SysComType com;
    // create minimal triangle
    re::engine::render::ColorVertex v1;
    v1.position = glm::vec3(0, 0, 0);
    v1.color = glm::vec4(1, 0, 0, 1);
    re::engine::render::ColorVertex v2;
    v2.position = glm::vec3(1, 0, 0);
    v2.color = glm::vec4(0, 1, 0, 1);
    re::engine::render::ColorVertex v3;
    v3.position = glm::vec3(0, 1, 0);
    v3.color = glm::vec4(0, 0, 1, 1);
    com.vertexs.push_back(v1);
    com.vertexs.push_back(v2);
    com.vertexs.push_back(v3);
    com.indexes.push_back(0);
    com.indexes.push_back(1);
    com.indexes.push_back(2);
    m_world->AddComponent<SysComType>(m_entity, com);
  }
  // Refresh inspector view
  InspectEntity(m_entity);
}

}  // namespace re::editor
