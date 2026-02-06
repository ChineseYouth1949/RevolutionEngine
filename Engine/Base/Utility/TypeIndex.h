namespace re::detail {
struct FNV1a64 {
  static constexpr uint64_t offset_basis = 14695981039346656037ull;
  static constexpr uint64_t prime = 1099511628211ull;

  // 递归实现 constexpr 字符串哈希
  static constexpr uint64_t hash(const char* str, uint64_t last_hash = offset_basis) {
    return (*str == '\0') ? last_hash : hash(str + 1, (last_hash ^ static_cast<uint64_t>(*str)) * prime);
  }
};

template <typename T>
constexpr uint64_t GetTypeHash() {
  // MSVC 使用 __FUNCSIG__, GCC/Clang 使用 __PRETTY_FUNCTION__
#if defined(_MSC_VER)
  return FNV1a64::hash(__FUNCSIG__);
#else
  return FNV1a64::hash(__PRETTY_FUNCTION__);
#endif
}
}  // namespace re::detail
