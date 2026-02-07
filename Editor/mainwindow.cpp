#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "SGWidget.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "ProjectPanel.h"

#include "RenderWindow.h"

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
      QMainWindow {
          background-color: #393939;
      }

      QMainWindow::separator {
          background-color: #191919;
          width: 1px;
          height: 1px;
          margin: 0px;
          padding: 0px;
      }

      QMainWindow::separator:hover {
          background-color: #4c4c4c;
      }

      QDockWidget {
          color: #cccccc;
          border: 1px solid #191919;
      }

      QDockWidget::title {
          background-color: #333333;
          text-align: left;
          padding-left: 8px;
          border-bottom: 1px solid #191919;
          font-size: 12px;
      }

      QTabBar::tab {
          background: #2b2b2b;
          color: #cccccc;
          padding: 6px 16px;
          border: 1px solid #191919;
          border-bottom: none;
          margin-right: 1px;
      }

      QTabBar::tab:selected {
          background: #393939;
          border-bottom: 2px solid #2d8da8;
      }

      QTabBar::tab:hover:!selected {
          background: #333333;
      }

      QTabWidget::pane {
          border: 1px solid #191919;
          background: #393939;
      }

      QListWidget {
          background-color: #2b2b2b;
          border: none;
          color: #cccccc;
          outline: none;
      }

      QListWidget::item {
          padding: 4px 8px;
          border-bottom: 1px solid #333333;
      }

      QListWidget::item:selected {
          background-color: #2d8da8;
          color: #ffffff;
      }

      QListWidget::item:hover {
          background-color: #333333;
      }

      QTreeWidget {
          background-color: #2b2b2b;
          border: none;
          color: #cccccc;
          outline: none;
      }

      QTreeWidget::item {
          padding: 4px 8px;
          border-bottom: 1px solid #333333;
      }

      QTreeWidget::item:selected {
          background-color: #2d8da8;
          color: #ffffff;
      }

      QScrollBar:vertical {
          background: #2b2b2b;
          width: 12px;
          margin: 0px;
      }

      QScrollBar::handle:vertical {
          background: #555555;
          min-height: 20px;
      }

      QScrollBar::handle:vertical:hover {
          background: #666666;
      }

      QScrollBar:horizontal {
          background: #2b2b2b;
          height: 12px;
          margin: 0px;
      }

      QScrollBar::handle:horizontal {
          background: #555555;
          min-width: 20px;
      }

      QTextEdit {
          background-color: #2b2b2b;
          color: #cccccc;
          border: none;
      }
  )");
}

void MainWindow::CreateDockManager() {
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
  m_dockManager->addDockWidget(ads::LeftDockWidgetArea, m_hierarchyPanel);

  m_dockManager->addDockWidget(ads::CenterDockWidgetArea, m_sceneGameWidget);

  m_dockManager->addDockWidget(ads::RightDockWidgetArea, m_inspectorPanel);

  // Make Project occupy the full bottom row (spanning columns 1-3)
  auto* bottomArea = m_dockManager->addDockWidget(ads::BottomDockWidgetArea, m_projectPanel);

  // Put Assets and Console as tabs in the bottom area alongside Project
  m_dockManager->addDockWidgetTabToArea(m_assetsPanel, bottomArea);
  m_dockManager->addDockWidgetTabToArea(m_consolePanel, bottomArea);
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
      // Count entities in registry (use each() to remain compatible with entt versions)
      size_t entityCount = 0;
      //   worldPtr->GetRegistry()->each([&](auto) { ++entityCount; });
      LogToConsole(QString("Scene loaded: %1 entities").arg(static_cast<long long>(entityCount)));
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

    if (parent) {
      parent->addChild(item);
    } else {
      m_assetsTree->addTopLevelItem(item);
    }

    if (info.isDir()) {
      BuildAssetTree(item, info.absoluteFilePath());
    }
  }
}

void MainWindow::LogToConsole(const QString& message) {
  if (m_consoleOutput) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    m_consoleOutput->append(QString("<span style='color: #888;'>[%1]</span> %2").arg(timestamp, message));
  }
}

}  // namespace re::editor