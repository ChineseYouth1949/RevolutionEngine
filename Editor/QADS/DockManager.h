#pragma once

#include <QMainWindow>

namespace QADS {

// Lightweight adapter so existing code can include "QADS/DockManager.h"
// In your environment replace this with the real QADS header when available.
class DockManager : public QMainWindow {
  Q_OBJECT
 public:
  using QMainWindow::QMainWindow;
};

}  // namespace QADS
