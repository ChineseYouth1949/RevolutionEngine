#include "HierarchyPanel.h"
#include "InspectorPanel.h"

#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QDockWidget>

namespace re::editor {

HierarchyPanel::HierarchyPanel(QWidget* parent) : ads::CDockWidget("Hierarchy", parent) {
  setObjectName("HierarchyPanel");
  QWidget* w = new QWidget(this);
  QVBoxLayout* l = new QVBoxLayout(w);
  l->setContentsMargins(0, 0, 0, 0);

  m_list = new QListWidget(this);
  m_list->setContextMenuPolicy(Qt::CustomContextMenu);
  l->addWidget(m_list);

  m_menu = new QMenu(m_list);
  m_menu->addAction("Create", this, &HierarchyPanel::OnCreateEntity);
  m_menu->addAction("Delete", this, &HierarchyPanel::OnDeleteEntity);

  setWidget(w);

  // Allow docking on left/right and enable moving/floating if underlying widget supports QDockWidget API
  if (auto dock = qobject_cast<QDockWidget*>(this)) {
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    dock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
  }

  connect(m_list, &QListWidget::itemClicked, this, &HierarchyPanel::OnItemClicked);
  connect(m_list, &QListWidget::customContextMenuRequested, this, &HierarchyPanel::OnContextRequested);
}

void HierarchyPanel::SetWorld(engine::ecs::World* world) {
  m_world = world;
  Refresh();
}

void HierarchyPanel::Refresh() {
  RefreshEntityList();
}

void HierarchyPanel::RefreshEntityList() {
  m_list->clear();
  if (!m_world)
    return;

  auto reg = m_world->GetRegistry();
  if (!reg)
    return;

  for (auto entity : reg->view<engine::ecs::Entity>()) {
    uint32_t id = static_cast<uint32_t>(entity);
    QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(std::to_string(id)), m_list);
    item->setData(Qt::UserRole, QVariant::fromValue(id));
    m_list->addItem(item);
  }
}

void HierarchyPanel::OnItemClicked(QListWidgetItem* item) {
  if (!item)
    return;
  uint32_t id = item->data(Qt::UserRole).toUInt();
  m_selected = static_cast<engine::ecs::Entity>(id);
  if (m_inspector)
    m_inspector->InspectEntity(m_selected);
}

void HierarchyPanel::OnContextRequested(const QPoint& pos) {
  m_menu->exec(m_list->mapToGlobal(pos));
}

void HierarchyPanel::OnCreateEntity() {
  if (!m_world)
    return;
  auto e = m_world->CreateEntity();
  RefreshEntityList();
}

void HierarchyPanel::OnDeleteEntity() {
  if (!m_world)
    return;
  if (m_selected == entt::null)
    return;
  m_world->DestroyEntity(m_selected);
  m_selected = entt::null;
  RefreshEntityList();
  if (m_inspector)
    m_inspector->ClearInspection();
}

}  // namespace re::editor
