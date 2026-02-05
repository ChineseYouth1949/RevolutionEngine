#pragma once

#include "Pass.h"

#include "Engine/ECS/Resource/ResourceVisitor.h"
#include "Engine/ECS/World/RegistryVisitor.h"

namespace re::engine::ecs {
class World;

template <typename T>
class ReadResource {};
template <typename T>
class WriteResource {};

template <typename T>
class ReadComponent {};
template <typename T>
class WriteComponent {};

class RE_API SystemPass : public Pass {
 public:
  SystemPass(PassId id = PassIdFactory::Null, const string& name = "") : Pass(id, name) {}

  RE_FINLINE void Init(World* world) {
    m_ResourceVisitor = world->GetResourceManager()->RequestResourceVisitor();
    m_RegistryVisitor = world->RequestRegistryVisitor();
  }

  template <typename T>
  RE_FINLINE void SetReadResource() {
    static const PassMutexId mutexId = PassIdFactory::GetClassId<ReadResource<T>>();
    AddMutexId(mutexId);
  }

  template <typename T>
  RE_FINLINE void SetWriteResource() {
    static const PassMutexId mutexId = PassIdFactory::GetClassId<WriteResource<T>>();
    AddMutexId(mutexId);
  }

  template <typename T>
  RE_FINLINE void SetReadComponent() {
    static const PassMutexId mutexId = PassIdFactory::GetClassId<ReadComponent<T>>();
    AddMutexId(mutexId);
  }

  template <typename T>
  RE_FINLINE void SetWriteComponent() {
    static const PassMutexId mutexId = PassIdFactory::GetClassId<WriteComponent<T>>();
    AddMutexId(mutexId);
  }

 protected:
  ResourceVisitor m_ResourceVisitor;
  RegistryVisitor m_RegistryVisitor;
};
}  // namespace re::engine::ecs