#include "TypeId.h"

namespace re::engine::utility {
size_t TypeId::Next() {
  static size_t count = 0;
  return count++;
}
}  // namespace re::engine::utility