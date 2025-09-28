#pragma once

namespace RE::Core {

class MemoryManager {
 public:
  static MemoryManager& Instance();

  void* Malloc(size_t Size, uint32_t Alignment = 0) = 0;

 private:
  MemoryManager();
  ~MemoryManager();
};

}  // namespace RE::Core