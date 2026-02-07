#include "World.h"
#include "RegistryVisitor.h"

namespace re::engine::ecs {
World::World() {
  m_ResourceManager = GAlloc::make_unique<ResourceManager>();
}
World::~World() {}

void World::Submit(RegistryCommandBuffer&& buffer) {
  std::lock_guard lock(m_SumbitMutex);
  m_CommandBuffers.push_back(std::move(buffer));
}
void World::Flush() {
  for (auto& cmb : m_CommandBuffers) {
    auto& createEntitys = cmb.GetCreateEntitys();
    auto& destryEntitys = cmb.GetDestroyEntitys();
    auto& destryDelyEntitys = cmb.GetDestroyDelyEntitys();

    auto& opOrders = cmb.GetOpOrders();
    for (auto& op : opOrders) {
      if (op.first == RegistryCommandBuffer::OpType::Create) {
        *(createEntitys[op.second]) = CreateEntity();
      } else if (op.first == RegistryCommandBuffer::OpType::Destroy) {
        DestroyEntity(destryEntitys[op.second]);
      } else if (op.first == RegistryCommandBuffer::OpType::DestryDelay) {
        DestroyEntity(destryDelyEntitys[op.second], true);
      }
    }

    cmb.Reset();
  }

  m_CommandBuffers.clear();
}

RegistryVisitor World::RequestRegistryVisitor() {
  return RegistryVisitor(this);
}

}  // namespace re::engine::ecs