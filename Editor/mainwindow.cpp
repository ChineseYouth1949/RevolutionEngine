#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "SGWidget.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "ProjectPanel.h"
#include "RenderWindow.h"

#include <QCoreApplication>
#include <QADS/DockAreaWidget.h>
#include <QADS/DockSplitter.h>

namespace re::editor {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  setWindowTitle("Revolution Editor");
  resize(1920, 1080);

  ApplyUnityStyle();
  CreateDockManager();
  CreatePanels();
  SetupUnityLayout();
  InitializeWorldConnections();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::ApplyUnityStyle() {
  this->setStyleSheet(R"(
      QMainWindow { background-color: #393939; }
      QMainWindow::separator { background-color: #191919; width: 1px; height: 1px; }
      QDockWidget { color: #cccccc; border: 1px solid #191919; }
      QDockWidget::title { background-color: #333333; text-align: left; padding-left: 8px; border-bottom: 1px solid #191919; font-size: 12px; }
      QTabBar::tab { background: #2b2b2b; color: #cccccc; padding: 6px 16px; border: 1px solid #191919; border-bottom: none; margin-right: 1px; }
      QTabBar::tab:selected { background: #393939; border-bottom: 2px solid #2d8da8; }
      QTabWidget::pane { border: 1px solid #191919; background: #393939; }
      QTreeWidget { background-color: #2b2b2b; border: none; color: #cccccc; outline: none; }
      QTreeWidget::item:selected { background-color: #2d8da8; color: #ffffff; }
      QScrollBar:vertical { background: #2b2b2b; width: 12px; }
      QScrollBar::handle:vertical { background: #555555; min-height: 20px; }
      QTextEdit { background-color: #2b2b2b; color: #cccccc; border: none; }
  )");
}

void MainWindow::CreateDockManager() {
  ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, true);
  m_dockManager = new ads::CDockManager(this);
  setCentralWidget(m_dockManager);
}

void MainWindow::CreatePanels() {
  m_hierarchyPanel = new re::editor::HierarchyPanel(this);
  m_hierarchyPanel->setObjectName("HierarchyPanel");

  m_inspectorPanel = new re::editor::InspectorPanel(this);
  m_inspectorPanel->setObjectName("InspectorPanel");

  m_projectPanel = new ProjectPanel(this);
  m_projectPanel->setObjectName("ProjectPanel");

  m_sceneGameWidget = new SGWidget("Scene/Game", this);
  m_sceneGameWidget->setObjectName("SceneGameWidget");

  m_consolePanel = new ads::CDockWidget("Console", this);
  m_consolePanel->setObjectName("ConsolePanel");
  m_consoleOutput = new QTextEdit(m_consolePanel);
  m_consoleOutput->setReadOnly(true);
  m_consolePanel->setWidget(m_consoleOutput);

  m_assetsPanel = new ads::CDockWidget("Assets", this);
  m_assetsPanel->setObjectName("AssetsPanel");
  m_assetsTree = new QTreeWidget(m_assetsPanel);
  m_assetsTree->setHeaderHidden(true);
  m_assetsPanel->setWidget(m_assetsTree);
}

void MainWindow::SetupUnityLayout() {
  // 1. 创建基础停靠结构
  ads::CDockAreaWidget* sceneArea = m_dockManager->addDockWidget(ads::CenterDockWidgetArea, m_sceneGameWidget);
  ads::CDockAreaWidget* hierarchyArea = m_dockManager->addDockWidget(ads::LeftDockWidgetArea, m_hierarchyPanel, sceneArea);
  ads::CDockAreaWidget* inspectorArea = m_dockManager->addDockWidget(ads::RightDockWidgetArea, m_inspectorPanel, sceneArea);
  ads::CDockAreaWidget* projectArea = m_dockManager->addDockWidget(ads::BottomDockWidgetArea, m_projectPanel);

  // 2. 调整水平比例 (15% : 70% : 15%)
  if (sceneArea) {
    if (auto* horizSplitter = sceneArea->parentSplitter()) {
      horizSplitter->setStretchFactor(horizSplitter->indexOf(hierarchyArea), 15);
      horizSplitter->setStretchFactor(horizSplitter->indexOf(sceneArea), 70);
      horizSplitter->setStretchFactor(horizSplitter->indexOf(inspectorArea), 15);

      // --- 关键修复：调整垂直比例 (80% : 20%) ---
      // 在 ADS 中，Splitter 的 parent 是一个 QWidget (通常是上级 Splitter 或 DockContainer)
      if (auto* parentWidget = horizSplitter->parentWidget()) {
        if (auto* vertSplitter = qobject_cast<ads::CDockSplitter*>(parentWidget)) {
          vertSplitter->setStretchFactor(vertSplitter->indexOf(horizSplitter), 80);
          vertSplitter->setStretchFactor(vertSplitter->indexOf(projectArea), 20);
        }
      }
    }
  }

  // 3. 合并页签
  if (projectArea) {
    m_dockManager->addDockWidgetTabToArea(m_assetsPanel, projectArea);
    m_dockManager->addDockWidgetTabToArea(m_consolePanel, projectArea);
  }
}

void MainWindow::InitializeWorldConnections() {
  auto* sceneWindow = m_sceneGameWidget->GetSceneWindow();
  if (sceneWindow && sceneWindow->GetScene()) {
    auto scenePtr = sceneWindow->GetScene();
    auto worldPtr = scenePtr->GetWorld();

    if (worldPtr) {
      m_hierarchyPanel->SetWorld(worldPtr);
      m_inspectorPanel->SetWorld(worldPtr);
      m_hierarchyPanel->SetInspector(m_inspectorPanel);

      QString appPathQt = QCoreApplication::applicationDirPath();
      QString luaDir = appPathQt + "/Resource";
      m_projectPanel->SetRootPath(luaDir);

      SyncAssetsWithProject(luaDir);
      LogToConsole("Editor initialized successfully");
    }
  }
}

void MainWindow::SyncAssetsWithProject(const QString& rootPath) {
  m_assetsRoot = rootPath;
  m_assetsTree->clear();
  QDir dir(rootPath);
  if (!dir.exists())
    return;
  BuildAssetTree(nullptr, rootPath);
}

void MainWindow::BuildAssetTree(QTreeWidgetItem* parent, const QString& path) {
  QDir dir(path);
  QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QFileInfo& info : entries) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, info.fileName());
    if (parent)
      parent->addChild(item);
    else
      m_assetsTree->addTopLevelItem(item);
    if (info.isDir())
      BuildAssetTree(item, info.absoluteFilePath());
  }
}

void MainWindow::LogToConsole(const QString& message) {
  if (m_consoleOutput) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_consoleOutput->append(QString("<span style='color: #888;'>[%1]</span> %2").arg(timestamp, message));
  }
}

}  // namespace re::editor
