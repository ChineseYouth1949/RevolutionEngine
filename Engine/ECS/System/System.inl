#pragma once

#include "System.h"

#include "Engine/ECS/World/World.h"

namespace re::engine::ecs {
template <typename ComponentType>
bool System::PollAddComponent() {
  auto reg = m_World->GetRegistry();

  auto& stateStorage = reg->storage<AddComponentTag<ComponentType>>();
  if (stateStorage.empty()) {
    return false;
  }

  auto& comStorage = reg->storage<ComponentType>();

  vector<Entity> entities;
  vector<ComponentType> components;
  entities.reserve(stateStorage.size());
  components.reserve(stateStorage.size());

  for (auto [e, addCom] : stateStorage.each()) {
    entities.push_back(e);
    components.push_back(std::move(addCom.data));
  }

  comStorage.insert(entities.begin(), entities.end(), components.begin());
  stateStorage.clear();

  return true;
}

template <typename ComponentType>
bool System::PollDelComponent() {
  auto reg = m_World->GetRegistry();

  auto& stateStorage = reg->storage<DelComponentTag<ComponentType>>();
  if (stateStorage.empty()) {
    return false;
  }

  auto& comStorage = reg->storage<ComponentType>();

  comStorage.remove(stateStorage.begin(), stateStorage.end());
  stateStorage.clear();

  return true;
}

template <typename ComponentType>
bool System::PollChangeComponent() {
  auto reg = m_World->GetRegistry();

  auto& stateStorage = reg->storage<ChangeComponentTag<ComponentType>>();
  if (stateStorage.empty()) {
    return false;
  }

  stateStorage.clear();

  return true;
}

}  // namespace re::engine::ecs