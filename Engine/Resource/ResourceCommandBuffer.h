#pragma once

#include "Resource.h"

#include "Engine/Type/Container.h"

#include <functional>

namespace re::engine::resource {
using namespace continer;

struct ResourceId {
  static constexpr uint32_t Invalid = 0xFFFFFFFFU;

  ResourceId(uint32_t _value = Invalid, uint32_t _version = 0) : value(_value), version(_version) {}

  bool IsNull() const { return value == Invalid; }
  bool operator==(const ResourceId& other) const { return value == other.value && version == other.version; }

  uint32_t value;
  uint32_t version;
};

using ResourceOperate = std::function<void(Resource&)>;

class RE_API ResCommandBuffer {
  friend class ResourceManager;

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
        m_ChangeResourceOpers(std::move(other.m_ChangeResourceOpers)),
        m_Orders(std::move(other.m_Orders)) {}

  ResCommandBuffer& operator=(ResCommandBuffer&& other) noexcept {
    if (this != &other) {
      m_AddResources = std::move(other.m_AddResources);
      m_ChangeResources = std::move(other.m_ChangeResources);
      m_RemoveResources = std::move(other.m_RemoveResources);
      m_ChangeResourceOpers = std::move(other.m_ChangeResourceOpers);
      m_Orders = std::move(other.m_Orders);
    }
    return *this;
  }

  shared_ptr<ResourceId> AddResource(Resource& res) {
    auto idPtr = GAlloc::make_shared<ResourceId>(0);
    m_Orders.push_back({OpType::Add, m_AddResources.size()});
    m_AddResources.emplace_back(idPtr, std::move(res));
    return idPtr;
  }

  template <typename F>
  void ChangeResource(ResourceId id, F&& op) {
    m_Orders.push_back({OpType::Change, m_ChangeResources.size()});
    m_ChangeResources.emplace_back(id, std::forward<F>(op));
  }

  void RemoveResource(ResourceId id) {
    m_Orders.push_back({OpType::Remove, m_RemoveResources.size()});
    m_RemoveResources.push_back(id);
  }

  void Reset() {
    m_AddResources.clear();
    m_ChangeResources.clear();
    m_RemoveResources.clear();
    m_Orders.clear();
  }

 private:
  RE_FINLINE vector<stl::pair<shared_ptr<ResourceId>, Resource>>& GetAddResources() { return m_AddResources; }
  RE_FINLINE vector<stl::pair<ResourceId, ResourceOperate>>& GetChangeResources() { return m_ChangeResources; }
  RE_FINLINE vector<ResourceId>& GetRemoveResources() { return m_RemoveResources; }
  RE_FINLINE vector<stl::pair<OpType, uint32_t>>& GetOpOrders() { return m_Orders; }

  vector<stl::pair<shared_ptr<ResourceId>, Resource>> m_AddResources;
  vector<stl::pair<ResourceId, Resource>> m_ChangeResources;
  vector<stl::pair<ResourceId, ResourceOperate>> m_ChangeResourceOpers;
  vector<ResourceId> m_RemoveResources;
  vector<stl::pair<OpType, uint32_t>> m_Orders;
};
}  // namespace re::engine::resource