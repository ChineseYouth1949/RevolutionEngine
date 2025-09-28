#pragma once

#define UNICODE

#include <windows.h>
#include <string>

namespace Engine {

class Window {
 public:
  Window(UINT width, UINT height, std::wstring title);
  virtual ~Window();

  virtual void ParseCommandLineArgs(WCHAR* argv[], int argc) {}
  virtual void OnInit() {}
  virtual void OnPaint() {}
  virtual void OnDestroy() {}

  virtual void OnKeyDown(UINT8 key) {}
  virtual void OnKeyUp(UINT8 key) {}

  virtual void OnMouseMove(UINT x, UINT y) {}
  virtual void OnLeftButtonDown(UINT x, UINT y) {}
  virtual void OnLeftButtonUp(UINT x, UINT y) {}
  virtual void OnRightButtonDown(UINT x, UINT y) {}
  virtual void OnRightButtonUp(UINT x, UINT y) {}

  virtual void OnSizeChanged(UINT width, UINT height, bool minimized) {}

  UINT GetWidth() const;
  UINT GetHeight() const;
  void SetSize(UINT width, UINT height);

  std::wstring GetTitle() const;
  void SetTitle(std::wstring title);

  HWND GetHWND();

 private:
  UINT m_width;
  UINT m_height;
  std::wstring m_title;

  HWND m_hwnd;

  friend class Win32Applicaiotn;
};

class Win32Applicaiotn {
 public:
  static int Run(Window* window, HINSTANCE hInstance, int nCmdShow);
};

}  // namespace Engine