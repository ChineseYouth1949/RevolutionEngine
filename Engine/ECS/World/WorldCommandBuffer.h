#pragma once

#include "Entity.h"

namespace re::engine::ecs {
class RE_API WorldCommandBuffer {
 public:
  enum struct OpType : uint8_t { Create = 0, Destroy, DestryDelay };

  WorldCommandBuffer() = default;

  WorldCommandBuffer(const WorldCommandBuffer&) = delete;
  WorldCommandBuffer& operator=(const WorldCommandBuffer&) = delete;

  WorldCommandBuffer(WorldCommandBuffer&& other) noexcept
      : m_CreateEntitys(std::move(other.m_CreateEntitys)),
        m_DestroyEntitys(std::move(other.m_DestroyEntitys)),
        m_DestroyDelayEntitys(std::move(other.m_DestroyDelayEntitys)),
        m_Orders(std::move(other.m_Orders)) {}

  WorldCommandBuffer& operator=(WorldCommandBuffer&& other) noexcept {
    if (this != &other) {
      m_CreateEntitys = std::move(other.m_CreateEntitys);
      m_DestroyEntitys = std::move(other.m_DestroyEntitys);
      m_DestroyDelayEntitys = std::move(other.m_DestroyDelayEntitys);
      m_Orders = std::move(other.m_Orders);
    }
    return *this;
  }

  shared_ptr<Entity> CreateEntity() {
    m_Orders.push_back({OpType::Create, m_CreateEntitys.size()});
    auto& ePtr = m_CreateEntitys.emplace_back();
    ePtr = GAlloc::make_shared<Entity>(entt::null);
    return ePtr;
  }

  void DestroyEntity(Entity e) {
    m_Orders.push_back({OpType::Destroy, m_DestroyEntitys.size()});
    m_DestroyEntitys.push_back(e);
  }

  void DestroyEntityDely(Entity e) {
    m_Orders.push_back({OpType::DestryDelay, m_DestroyDelayEntitys.size()});
    m_DestroyDelayEntitys.push_back(e);
  }

  RE_FINLINE vector<shared_ptr<Entity>>& GetCreateEntitys() { return m_CreateEntitys; }
  RE_FINLINE vector<Entity>& GetDestroyEntitys() { return m_DestroyEntitys; }
  RE_FINLINE vector<Entity>& GetDestroyDelyEntitys() { return m_DestroyDelayEntitys; }
  RE_FINLINE vector<stl::pair<OpType, uint32_t>>& GetOpOrders() { return m_Orders; }

  void Reset() {
    m_CreateEntitys.clear();
    m_DestroyEntitys.clear();
    m_Orders.clear();
  }

 private:
  vector<shared_ptr<Entity>> m_CreateEntitys;
  vector<Entity> m_DestroyEntitys;
  vector<Entity> m_DestroyDelayEntitys;
  vector<stl::pair<OpType, uint32_t>> m_Orders;
};
}  // namespace re::engine::ecs