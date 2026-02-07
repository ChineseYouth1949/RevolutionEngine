#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDockWidget>
#include "SGWidget.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "ProjectPanel.h"

#include "RenderWindow.h"

namespace re::editor {
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  this->setStyleSheet(R"(
      /* 1. 全局背景色 - Unity 经典的暗灰色 */
      QMainWindow {
          background-color: #383838;
      }

      /* 2. 重点：定制面板之间的分割线 */
      QMainWindow::separator {
          background-color: #1a1a1a; /* 分割线颜色 */
          width: 2px;               /* 垂直分割线宽度 */
          height: 2px;              /* 水平分割线高度 */
          margin: 0px;
          padding: 0px;
      }

      /* 鼠标悬停在分割线上时的效果 */
      QMainWindow::separator:hover {
          background-color: #4c4c4c;
      }

      /* 3. 定制 DockWidget 的边框和标题栏 */
      QDockWidget {
          color: #dcdcdc;
          titlebar-close-icon: url(close.png); /* 可选：自定义关闭按钮 */
          titlebar-normal-icon: url(float.png);
          border: 1px solid #1a1a1a; /* 面板的外边框 */
      }

      /* 定制标题栏的外观 */
      QDockWidget::title {
          background-color: #2b2b2b;
          text-align: left;
          padding-left: 10px;
          border-bottom: 1px solid #1a1a1a;
      }

      /* 4. 当两个 Dock 堆叠成 Tab 时的样式 (类似 Unity 面板切换) */
      QTabBar::tab {
          background: #2b2b2b;
          color: #dcdcdc;
          padding: 6px 12px;
          border: 1px solid #1a1a1a;
          border-bottom: none;
      }

      QTabBar::tab:selected {
          background: #383838; /* 选中的 Tab 颜色与主窗口一致 */
          border-bottom: 2px solid #4a90e2; /* 类似虚幻或现代 Unity 的选中高亮线 */
      }
  )");

  // 1. 将中心控件设为空
  setCentralWidget(nullptr);

  // 2. 开启嵌套和合并属性（这是灵魂）
  setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);

  auto* hierarchy = new re::editor::HierarchyPanel(this);
  auto* inspector = new re::editor::InspectorPanel(this);
  auto* project = new ProjectPanel(this);

  auto* scene = new SGWidget("", this);  // 包含 DX12 QWindow

  // --- 1. 先建立上半部分的布局 (Hierarchy + Scene) ---
  addDockWidget(Qt::LeftDockWidgetArea, hierarchy);
  splitDockWidget(hierarchy, scene, Qt::Horizontal);  // 此时上半部分是 [H | S]

  // --- 2. 建立右侧布局 ---
  splitDockWidget(scene, inspector, Qt::Horizontal);  // 此时整体是 [H | S | I]

  // --- 3. 关键：将 Project 垂直切分到【上半部分整体】的下面 ---
  // 我们不再以 scene 为基准，而是以最左侧的 hierarchy 为基准进行垂直切分
  // 并且必须开启嵌套选项
  setDockOptions(QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks);

  // 尝试将 project 放置在 hierarchy 的下方
  splitDockWidget(hierarchy, project, Qt::Vertical);
  // 将 World 传递给面板（SGWidget 已创建 RenderWindow 并初始化 Scene）
  if (scene && scene->GetSceneWindow() && scene->GetSceneWindow()->GetScene()) {
    auto scenePtr = scene->GetSceneWindow()->GetScene();
    auto worldPtr = scenePtr->GetWorld();
    if (worldPtr) {
      hierarchy->SetWorld(worldPtr);
      inspector->SetWorld(worldPtr);
      // set project root to app Resource folder by default
      QString appPathQt = QCoreApplication::applicationDirPath();
      QString luaDir = appPathQt + "/Resource";
      ProjectPanel* proj = static_cast<ProjectPanel*>(project);
      proj->SetRootPath(luaDir);
    }
  }
  // 连接面板
  hierarchy->SetInspector(inspector);

  // 如果 RenderWindow later provides World, panels will be set from there
}

MainWindow::~MainWindow() {
  delete ui;
}
}  // namespace re::editor