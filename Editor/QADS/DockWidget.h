#pragma once

#include <QDockWidget>

namespace QADS {

// Lightweight adapter so existing code can include "QADS/DockWidget.h"
// In your environment replace this with the real QADS header when available.
class DockWidget : public QDockWidget {
  Q_OBJECT
 public:
  using QDockWidget::QDockWidget;
};

}  // namespace QADS
