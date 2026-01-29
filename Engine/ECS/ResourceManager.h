#pragma once

#include "Resource.h"
#include "IdFactory.h"

namespace re::engine::ecs {
class RE_API ResourceManager {
 public:
  ResourceManager() { m_Resources.resize(ResIdFactory::sMaxId); }
  ~ResourceManager() { Clear(); }

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;

  template <typename T>
  RE_FINLINE T* GetResource() {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    RE_ASSERT(id < m_Resources.size());
    return m_Resources[id].Get<T>();
  }

  template <typename T>
  bool HasResource() {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    RE_ASSERT(id < m_Resources.size());
    return m_Resources[id].Get<T>() != nullptr;
  }

  template <typename T, typename... Args>
  T* CreateResource(Args&&... args) {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    RE_ASSERT(id < m_Resources.size());

    T* ptr = GAlloc::create<T>(std::forward<Args>(args)...);
    m_Resources[id].Create<T>(ptr);

    return ptr;
  }

  template <typename T>
  void RemoveResource() {
    const ResouceId id = ResIdFactory::GetTypeConstId<T>();
    RE_ASSERT(id < m_Resources.size());

    std::lock_guard<std::mutex> lock(m_PendingMutex);
    m_PendingIds.push_back(id);
  }

  void Clear() { m_Clear.store(true, std::memory_order_relaxed); }

  void Flush() {
    if (m_Clear.load(std::memory_order_relaxed)) {
      ClearImpl();
      m_Clear.store(false, std::memory_order_relaxed);
    } else {
      RemoveResourceImpl();
    }
  }

 private:
  void RemoveResourceImpl() {
    std::vector<size_t> idsToDestroy;
    {
      std::lock_guard<std::mutex> lock(m_PendingMutex);
      if (m_PendingIds.empty())
        return;
      idsToDestroy.swap(m_PendingIds);
    }

    for (size_t id : idsToDestroy) {
      m_Resources[id].DestroyNoLock();
    }
  }

  void ClearImpl() {
    {
      std::lock_guard<std::mutex> lock(m_PendingMutex);
      m_PendingIds.clear();
    }
    for (auto& res : m_Resources) {
      res.DestroyNoLock();
    }
  }

  std::vector<Resource> m_Resources;
  std::vector<size_t> m_PendingIds;
  std::mutex m_PendingMutex;
  std::atomic<bool> m_Clear{false};
};
}  // namespace re::engine::ecs