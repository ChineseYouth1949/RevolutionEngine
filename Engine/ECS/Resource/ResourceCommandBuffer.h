#pragma once

#include "Resource.h"

namespace re::engine::ecs {
using ResId = uint32_t;
using ResOperate = std::function<void(Resource&)>;

class RE_API ResCommandBuffer {
 public:
  enum struct OpType : uint8_t { Add = 0, Change, Remove };

  ResCommandBuffer() = default;
  ~ResCommandBuffer() = default;

  ResCommandBuffer(const ResCommandBuffer&) = delete;
  ResCommandBuffer& operator=(const ResCommandBuffer&) = delete;

  ResCommandBuffer(ResCommandBuffer&& other) noexcept
      : m_AddResources(std::move(other.m_AddResources)),
        m_ChangeResources(std::move(other.m_ChangeResources)),
        m_RemoveResources(std::move(other.m_RemoveResources)),
        m_Orders(std::move(other.m_Orders)) {}

  ResCommandBuffer& operator=(ResCommandBuffer&& other) noexcept {
    if (this != &other) {
      m_AddResources = std::move(other.m_AddResources);
      m_ChangeResources = std::move(other.m_ChangeResources);
      m_RemoveResources = std::move(other.m_RemoveResources);
      m_Orders = std::move(other.m_Orders);
    }
    return *this;
  }

  shared_ptr<ResId> AddResocue(Resource& res) {
    auto idPtr = GAlloc::make_shared<ResId>(0);
    m_Orders.push_back({OpType::Add, m_AddResources.size()});
    m_AddResources.emplace_back(idPtr, std::move(res));
    return idPtr;
  }

  template <typename F>
  void ChangeResocue(ResId id, F&& op) {
    m_Orders.push_back({OpType::Change, m_ChangeResources.size()});
    m_ChangeResources.emplace_back(id, std::forward<F>(op));
  }

  void RemoveResocue(ResId id) {
    m_Orders.push_back({OpType::Remove, m_RemoveResources.size()});
    m_RemoveResources.push_back(id);
  }

  void Reset() {
    m_AddResources.clear();
    m_ChangeResources.clear();
    m_RemoveResources.clear();
    m_Orders.clear();
  }

  RE_FINLINE vector<stl::pair<shared_ptr<ResId>, Resource>>& GetAddResources() { return m_AddResources; }
  RE_FINLINE vector<stl::pair<ResId, ResOperate>>& GetChangeResources() { return m_ChangeResources; }
  RE_FINLINE vector<ResId>& GetRemoveResources() { return m_RemoveResources; }
  RE_FINLINE vector<stl::pair<OpType, uint32_t>>& GetOpOrders() { return m_Orders; }

 private:
  vector<stl::pair<shared_ptr<ResId>, Resource>> m_AddResources;
  vector<stl::pair<ResId, ResOperate>> m_ChangeResources;
  vector<ResId> m_RemoveResources;
  vector<stl::pair<OpType, uint32_t>> m_Orders;
};
}  // namespace re::engine::ecs