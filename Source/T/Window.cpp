#include "Window.h"

namespace Engine {

Window::Window(UINT width, UINT height, std::wstring title) : m_width(width), m_height(height), m_title(title) {}

Window::~Window() {}

UINT Window::GetWidth() const {
  return m_width;
}
UINT Window::GetHeight() const {
  return m_height;
}
void Window::SetSize(UINT width, UINT height) {
  m_width = width;
  m_height = height;
}

std::wstring Window::GetTitle() const {
  return m_title;
}
void Window::SetTitle(std::wstring title) {
  SetWindowTextW(m_hwnd, title.c_str());
  m_title = title;
}

HWND Window::GetHWND() {
  return m_hwnd;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int Win32Applicaiotn::Run(Window* window, HINSTANCE hInstance, int nCmdShow) {
  try {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    window->ParseCommandLineArgs(argv, argc);
    LocalFree(argv);

    WNDCLASSEX windowClass = {0};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"Window";
    RegisterClassEx(&windowClass);

    RECT windowRect = {0, 0, static_cast<LONG>(window->GetWidth()), static_cast<LONG>(window->GetHeight())};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    window->m_hwnd = CreateWindow(windowClass.lpszClassName, window->GetTitle().c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                  windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, window);

    window->OnInit();

    ShowWindow(window->GetHWND(), nCmdShow);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }

    window->OnDestroy();

    return static_cast<char>(msg.wParam);
  } catch (std::exception& e) {
    OutputDebugString(L"Application hit a problem: ");
    OutputDebugStringA(e.what());
    OutputDebugString(L"\nTerminating.\n");

    window->OnDestroy();
    return EXIT_FAILURE;
  }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

  switch (message) {
    case WM_CREATE: {
      LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
      SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    }
      return 0;

    case WM_PAINT:
      if (window) {
        window->OnPaint();
      }
      return 0;

    case WM_SIZE:
      if (window) {
        RECT clientRect = {};
        GetClientRect(hwnd, &clientRect);
        window->OnSizeChanged(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, wParam == SIZE_MINIMIZED);
      }
      return 0;

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

    case WM_KEYDOWN:
      if (window) {
        window->OnKeyDown(static_cast<UINT8>(wParam));
      }
      return 0;

    case WM_KEYUP:
      if (window) {
        window->OnKeyUp(static_cast<UINT8>(wParam));
      }
      return 0;

    case WM_MOUSEMOVE:
      if (window) {
        UINT x = LOWORD(lParam);
        UINT y = HIWORD(lParam);
        window->OnMouseMove(x, y);
      }
      return 0;

    case WM_LBUTTONDOWN:
      if (window) {
        UINT x = LOWORD(lParam);
        UINT y = HIWORD(lParam);
        window->OnLeftButtonDown(x, y);
      }
      return 0;

    case WM_LBUTTONUP:
      if (window) {
        UINT x = LOWORD(lParam);
        UINT y = HIWORD(lParam);
        window->OnLeftButtonUp(x, y);
      }
      return 0;

    case WM_RBUTTONDOWN:
      if (window) {
        UINT x = LOWORD(lParam);
        UINT y = HIWORD(lParam);
        window->OnRightButtonDown(x, y);
      }
      return 0;

    case WM_RBUTTONUP:
      if (window) {
        UINT x = LOWORD(lParam);
        UINT y = HIWORD(lParam);
        window->OnRightButtonUp(x, y);
      }
      return 0;
  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}

}  // namespace Engine