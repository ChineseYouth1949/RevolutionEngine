#pragma once

#include <windows.h>
#include <commctrl.h>  // for treeview control

namespace SceneTreeView {

ATOM UIRegisterClass(HINSTANCE hInstance);
BOOL InitWindow(HINSTANCE, int);

}  // namespace SceneTreeView