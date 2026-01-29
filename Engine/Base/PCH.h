#pragma once

#include "Macros.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP
#define NOMCX
#define NOSERVICE
#define NOHELP
#endif

#include <Windows.h>
#include <wrl/client.h>
#include <wrl/event.h>

#include "STL/STL.h"
#include <mutex>
#include <thread>
#include <assert.h>
#include <tchar.h>
#include <utility>
#include <cinttypes>
#include <typeindex>
#include <memory>
#include <malloc.h>
#include <chrono>

#include <dxgi1_6.h>
#include <dxcapi.h>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"

#include <d3dcompiler.h>
#include <dxgidebug.h>

#include <entt/entt.hpp>
#include <mimalloc.h>
#include <glm/glm.hpp>