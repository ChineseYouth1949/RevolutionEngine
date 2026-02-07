#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTreeWidget>
#include <QDir>
#include <QDateTime>
#include "QADS/DockManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace re::editor {

class HierarchyPanel;
class InspectorPanel;
class ProjectPanel;
class SGWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 private:
  void ApplyUnityStyle();
  void CreateDockManager();
  void CreatePanels();
  void SetupUnityLayout();
  void InitializeWorldConnections();
  void SyncAssetsWithProject(const QString& rootPath);
  void BuildAssetTree(QTreeWidgetItem* parent, const QString& path);
  void LogToConsole(const QString& message);

 private:
  Ui::MainWindow* ui;
  ads::CDockManager* m_dockManager{nullptr};

  re::editor::HierarchyPanel* m_hierarchyPanel{nullptr};
  re::editor::InspectorPanel* m_inspectorPanel{nullptr};
  re::editor::ProjectPanel* m_projectPanel{nullptr};
  SGWidget* m_sceneGameWidget{nullptr};

  ads::CDockWidget* m_consolePanel{nullptr};
  QTextEdit* m_consoleOutput{nullptr};
  ads::CDockWidget* m_assetsPanel{nullptr};
  QTreeWidget* m_assetsTree{nullptr};
  QString m_assetsRoot;
};
#endif  // MAINWINDOW_H
}