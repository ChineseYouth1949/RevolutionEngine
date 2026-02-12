#pragma once

#include <cinttypes>
#include <atomic>

namespace re::engine {
#define RE_UINT32_MAX 0xFFFFFFFFU

using uint8 = std::uint8_t;
using uint32 = std::uint32_t;

template <typename T>
using atomic = std::atomic<T>;

}  // namespace re::engine