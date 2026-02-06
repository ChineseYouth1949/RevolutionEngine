namespace re::engine::ecs {
PassId PassId::Create() {
  uint32_t id = sIdCounter.fetch_add(1, std::memory_order_relaxed);
  return PassId(id);
}

template <typename T>
PassId PassId::FromClass() {
  static const PassId id = Create();
  return id;
}

template <typename T>
uint32_t GetPassMutexTypeTag() {
  static const uint32_t tag = PassMutex::sTagCounter.fetch_add(1, std::memory_order_relaxed);
  return tag;
}

template <typename T>
PassMutex PassMutex::ReadResource() {
  static const uint32_t tag = GetPassMutexTypeTag<T>();
  PassMutex mutex(Type::ResourceAccess);
  mutex.data.accessType = AccessType::Read;
  mutex.data.accessTag = tag;
  return mutex;
}

template <typename T>
PassMutex PassMutex::WriteResource() {
  static const uint32_t tag = GetPassMutexTypeTag<T>();
  PassMutex mutex(Type::ResourceAccess);
  mutex.data.accessType = AccessType::ReadWrite;
  mutex.data.accessTag = tag;
  return mutex;
}

template <typename T>
PassMutex PassMutex::ReadCommand() {
  static const uint32_t tag = GetPassMutexTypeTag<T>();
  PassMutex mutex(Type::ComponentAccess);
  mutex.data.accessType = AccessType::Read;
  mutex.data.accessTag = tag;
  return mutex;
}

template <typename T>
PassMutex PassMutex::WriteCommand() {
  static const uint32_t tag = GetPassMutexTypeTag<T>();
  PassMutex mutex(Type::ComponentAccess);
  mutex.data.accessType = AccessType::ReadWrite;
  mutex.data.accessTag = tag;
  return mutex;
}

}  // namespace re::engine::ecs