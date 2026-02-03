#pragma once

#include "Resource.h"

namespace re::engine::ecs {
struct RE_API ResourceId {
  static constexpr uint32_t Null = std::numeric_limits<uint32_t>::max();

  ResourceId(uint32_t _value = Null) : value(_value) {}

  RE_FINLINE bool valid() const { return value != Null; }
  RE_FINLINE operator uint32_t() const { return value; }

  uint32_t value;
};

using ResOperate = std::function<void(Resource&)>;

class RE_API ResCommandBuffer {
 public:
  enum struct OpType : uint8_t { Add = 0, Change, Remove };

  ResCommandBuffer() = default;
  ~ResCommandBuffer() { Reset(); }

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

  shared_ptr<ResourceId> AddResocue(Resource& res) {
    auto idPtr = GAlloc::make_shared<ResourceId>(0);
    m_Orders.push_back({OpType::Add, m_AddResources.size()});
    m_AddResources.emplace_back(idPtr, std::move(res));
    return idPtr;
  }

  template <typename F>
  void ChangeResocue(ResourceId id, F&& op) {
    m_Orders.push_back({OpType::Change, m_ChangeResources.size()});
    m_ChangeResources.emplace_back(id, std::forward<F>(op));
  }

  void RemoveResocue(ResourceId id) {
    m_Orders.push_back({OpType::Remove, m_RemoveResources.size()});
    m_RemoveResources.push_back(id);
  }

  void Reset() {
    m_AddResources.clear();
    m_ChangeResources.clear();
    m_RemoveResources.clear();
    m_Orders.clear();
  }

  RE_FINLINE vector<stl::pair<shared_ptr<ResourceId>, Resource>>& GetAddResources() { return m_AddResources; }
  RE_FINLINE vector<stl::pair<ResourceId, ResOperate>>& GetChangeResources() { return m_ChangeResources; }
  RE_FINLINE vector<ResourceId>& GetRemoveResources() { return m_RemoveResources; }
  RE_FINLINE vector<stl::pair<OpType, uint32_t>>& GetOpOrders() { return m_Orders; }

 private:
  vector<stl::pair<shared_ptr<ResourceId>, Resource>> m_AddResources;
  vector<stl::pair<ResourceId, ResOperate>> m_ChangeResources;
  vector<ResourceId> m_RemoveResources;
  vector<stl::pair<OpType, uint32_t>> m_Orders;
};
}  // namespace re::engine::ecs