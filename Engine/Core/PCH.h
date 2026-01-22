#pragma once

#include "STL/STL.h"
#include <mutex>

#include <assert.h>
#include <tchar.h>
#include <utility>
#include <cinttypes>

#include <memory>
#include <malloc.h>
#include <mimalloc.h>

#if defined(_WIN32)
// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <wrl/client.h>
#include <wrl/event.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include "directx/d3d12.h"

#ifdef RE_DEBUG
#include <dxgidebug.h>
#endif
#endif  // _WIN32

#include <entt/entt.hpp>