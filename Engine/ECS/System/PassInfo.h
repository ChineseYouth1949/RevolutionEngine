#pragma once

#include "Engine/Macros.h"

#include <atomic>

namespace re::engine::ecs {
struct RE_API PassId {
  constexpr explicit PassId(uint32_t val = RE_UINT32_MAX) noexcept : value(val) {}

  RE_FINLINE constexpr bool IsNull() const noexcept { return value != RE_UINT32_MAX; }
  constexpr explicit operator bool() const noexcept { return IsNull(); }
  auto operator<=>(const PassId& other) const = default;

  uint32_t value;

  RE_FINLINE static PassId Create();

  template <typename T>
  static PassId FromClass();

 private:
  static std::atomic<uint32_t> sIdCounter;
};

struct PassMutex {
  enum class Type : uint8_t {
    None = 0,
    ResourceAccess = 1,
    ComponentAccess = 2,
  };
  enum class AccessType : uint8_t {
    Read = 0,
    ReadWrite,
  };

  PassMutex(Type _type = Type::None) : type(_type) {}

  constexpr bool IsNull() const noexcept { return type != Type::None; }
  constexpr explicit operator bool() const noexcept { return IsNull(); }
  auto operator<=>(const PassMutex& other) const = default;

  Type type;

  // ResourceAccess or ComponentAccess
  union {
    struct {
      AccessType accessType;
      uint32_t accessTag;
    };
  } data;

  template <typename T>
  static PassMutex ReadResource();

  template <typename T>
  static PassMutex WriteResource();

  template <typename T>
  static PassMutex ReadCommand();

  template <typename T>
  static PassMutex WriteCommand();

 private:
  static std::atomic<uint32_t> sTagCounter;
};
}  // namespace re::engine::ecs

#include "PassInfo.inl"

// EASTL hash specialization for PassId
namespace eastl {
template <>
struct hash<re::engine::ecs::PassId> {
  size_t operator()(const re::engine::ecs::PassId& id) const noexcept { return hash<uint32_t>()(id.value); }
};
}  // namespace eastl