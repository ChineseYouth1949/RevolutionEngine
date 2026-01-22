#pragma once

namespace re::core::ecs {
template <typename T>
class AddComponent {
  template <typename... Args>
  AddComponent(Args&&... args) : data(std::forward<Args>(args)...) {}
  T data;
};

template <typename T>
class ChangeComponent {};

template <typename T>
class DelComponent {};
}  // namespace re::core::ecs