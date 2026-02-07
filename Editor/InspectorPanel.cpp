#include "InspectorPanel.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QTimer>

#include "Engine/Render/System/ColorVertex.h"

namespace re::editor {

InspectorPanel::InspectorPanel(QWidget* parent) : ads::CDockWidget("Inspector", parent) {
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

  // Allow docking areas and enable movable/floatable if underlying widget is a QDockWidget
  if (auto dock = qobject_cast<QDockWidget*>(this)) {
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
  }

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
  auto* vlay = new QVBoxLayout(group);

  // Position
  auto* posLay = new QHBoxLayout();
  m_posX = new QDoubleSpinBox(group);
  m_posY = new QDoubleSpinBox(group);
  m_posZ = new QDoubleSpinBox(group);
  for (auto* s : {m_posX, m_posY, m_posZ}) {
    s->setRange(-1e6, 1e6);
    s->setDecimals(4);
    s->setSingleStep(0.1);
  }
  m_posX->setValue(t.position.x);
  m_posY->setValue(t.position.y);
  m_posZ->setValue(t.position.z);
  posLay->addWidget(new QLabel("Position", group));
  posLay->addWidget(m_posX);
  posLay->addWidget(m_posY);
  posLay->addWidget(m_posZ);
  vlay->addLayout(posLay);

  // Rotation
  auto* rotLay = new QHBoxLayout();
  m_rotX = new QDoubleSpinBox(group);
  m_rotY = new QDoubleSpinBox(group);
  m_rotZ = new QDoubleSpinBox(group);
  for (auto* s : {m_rotX, m_rotY, m_rotZ}) {
    s->setRange(-360.0, 360.0);
    s->setDecimals(4);
    s->setSingleStep(1.0);
  }
  m_rotX->setValue(t.rotation.x);
  m_rotY->setValue(t.rotation.y);
  m_rotZ->setValue(t.rotation.z);
  rotLay->addWidget(new QLabel("Rotation", group));
  rotLay->addWidget(m_rotX);
  rotLay->addWidget(m_rotY);
  rotLay->addWidget(m_rotZ);
  vlay->addLayout(rotLay);

  // Scale
  auto* scaleLay = new QHBoxLayout();
  m_scaleX = new QDoubleSpinBox(group);
  m_scaleY = new QDoubleSpinBox(group);
  m_scaleZ = new QDoubleSpinBox(group);
  for (auto* s : {m_scaleX, m_scaleY, m_scaleZ}) {
    s->setRange(-1e3, 1e3);
    s->setDecimals(4);
    s->setSingleStep(0.1);
  }
  m_scaleX->setValue(t.scale.x);
  m_scaleY->setValue(t.scale.y);
  m_scaleZ->setValue(t.scale.z);
  scaleLay->addWidget(new QLabel("Scale", group));
  scaleLay->addWidget(m_scaleX);
  scaleLay->addWidget(m_scaleY);
  scaleLay->addWidget(m_scaleZ);
  vlay->addLayout(scaleLay);

  m_contents->layout()->addWidget(group);

  // connect editingFinished to class slot to avoid reentrancy issues
  connect(m_posX, &QDoubleSpinBox::editingFinished, this, &InspectorPanel::OnTransformEdited);
  connect(m_posY, &QDoubleSpinBox::editingFinished, this, &InspectorPanel::OnTransformEdited);
  connect(m_posZ, &QDoubleSpinBox::editingFinished, this, &InspectorPanel::OnTransformEdited);
  connect(m_rotX, &QDoubleSpinBox::editingFinished, this, &InspectorPanel::OnTransformEdited);
  connect(m_rotY, &QDoubleSpinBox::editingFinished, this, &InspectorPanel::OnTransformEdited);
  connect(m_rotZ, &QDoubleSpinBox::editingFinished, this, &InspectorPanel::OnTransformEdited);
  connect(m_scaleX, &QDoubleSpinBox::editingFinished, this, &InspectorPanel::OnTransformEdited);
  connect(m_scaleY, &QDoubleSpinBox::editingFinished, this, &InspectorPanel::OnTransformEdited);
  connect(m_scaleZ, &QDoubleSpinBox::editingFinished, this, &InspectorPanel::OnTransformEdited);
}

void InspectorPanel::OnTransformEdited() {
  if (!m_world || m_entity == entt::null)
    return;

  engine::ecs::Transform nt;
  nt.position.x = static_cast<float>(m_posX->value());
  nt.position.y = static_cast<float>(m_posY->value());
  nt.position.z = static_cast<float>(m_posZ->value());
  nt.rotation.x = static_cast<float>(m_rotX->value());
  nt.rotation.y = static_cast<float>(m_rotY->value());
  nt.rotation.z = static_cast<float>(m_rotZ->value());
  nt.scale.x = static_cast<float>(m_scaleX->value());
  nt.scale.y = static_cast<float>(m_scaleY->value());
  nt.scale.z = static_cast<float>(m_scaleZ->value());

  // schedule component change (delay)
  m_world->ChangeComponent<engine::ecs::Transform>(m_entity, nt);

  // Defer UI refresh to avoid deleting widgets while they are emitting signals
  QTimer::singleShot(0, this, [this, e = m_entity]() {
    if (e != entt::null) {
      InspectEntity(e);
    }
  });
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
