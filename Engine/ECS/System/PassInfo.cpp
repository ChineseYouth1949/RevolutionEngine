#include "PassInfo.h"

namespace re::engine::ecs {
std::atomic<uint32_t> PassId::sIdCounter = 0;
std::atomic<uint32_t> PassMutex::sTagCounter = 0;
}  // namespace re::engine::ecs