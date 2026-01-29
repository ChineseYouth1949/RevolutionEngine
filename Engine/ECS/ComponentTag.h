#pragma once

namespace re::engine::ecs {
template <typename T>
struct AddComponentTag {
  template <typename... Args>
  AddComponentTag(Args&&... args) : data(std::forward<Args>(args)...) {}
  T data;
};

template <typename T>
struct ChangeComponentTag {
  template <typename... Args>
  ChangeComponentTag(Args&&... args) : data(std::forward<Args>(args)...) {}
  T data;
};

template <typename T>
struct DelComponentTag {};
}  // namespace re::engine::ecs