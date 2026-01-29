#pragma once

namespace re::engine::ecs {
template <typename T>
struct StateComponentAdd {
  template <typename... Args>
  StateComponentAdd(Args&&... args) : data(std::forward<Args>(args)...) {}
  T data;
};

template <typename T>
struct StateComponentChange {};

template <typename T>
struct StateComponentDel {};
}  // namespace re::engine::ecs