#include "World.h"

namespace re::engine::ecs {
World::World() {
  m_ResourceManager = GAlloc::make_unique<ResourceManager>();
}
World::~World() {}

void World::Submit(WorldCommandBuffer& buffer) {
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
      if (op.first == WorldCommandBuffer::OpType::Create) {
        *(createEntitys[op.second]) = CreateEntity();
      } else if (op.first == WorldCommandBuffer::OpType::Destroy) {
        DestryEntity(destryEntitys[op.second]);
      } else if (op.first == WorldCommandBuffer::OpType::DestryDelay) {
        DestryEntityDelay(destryDelyEntitys[op.second]);
      }
    }

    cmb.Reset();
  }

  m_CommandBuffers.clear();
}

}  // namespace re::engine::ecs