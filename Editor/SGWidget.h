#pragma once

#include "QADS/DockWidget.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace re::editor {
class RenderWindow;

class SGWidget : public ads::CDockWidget {
  Q_OBJECT
 public:
  explicit SGWidget(const QString& title, QWidget* parent);

  RenderWindow* GetSceneWindow() const { return m_sceneWindow; }
  RenderWindow* GetGameWindow() const { return m_gameWindow; }

  void resizeEvent(QResizeEvent* event);

 private:
  QTabWidget* m_tabWidget;
  RenderWindow* m_sceneWindow;
  RenderWindow* m_gameWindow;
};

}  // namespace re::editor