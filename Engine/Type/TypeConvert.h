#pragma once

namespace re::engine::container {
// Conversion between different types
template <typename SrcType, typename DstType>
RE_FINLINE DstType wstringToStringTemp(const SrcType& wstr) {
  std::wstring_view sv(wstr);
  if (sv.empty())
    return DstType();

  // Get required size (number of bytes) for UTF-8 conversion
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, sv.data(), static_cast<int>(sv.size()), nullptr, 0, nullptr, nullptr);
  if (size_needed <= 0)
    return DstType();

  // Allocate buffer and perform conversion once. Use resize to set size and
  // then trim any trailing null if present.
  DstType strTo;
  strTo.resize(size_needed);
  int written = WideCharToMultiByte(CP_UTF8, 0, sv.data(), static_cast<int>(sv.size()), strTo.data(), size_needed, nullptr, nullptr);
  if (written <= 0)
    return DstType();

  // WideCharToMultiByte may or may not append a null terminator depending on length
  if (!strTo.empty() && strTo.back() == '\0')
    strTo.pop_back();
  else
    strTo.resize(written);

  return strTo;
}
#define wstringToString(str) wstringToStringTemp<string, wstring>(str)
#define wstringToStringStl(str) wstringToStringTemp<stl::string, stl::wstring>(str)
#define wstringToStringStd(str) wstringToStringTemp<std::string, std::wstring>(str)

// Conversion between the same type
template <typename SrcType, typename DstType>
RE_FINLINE DstType stringCopyTemp(const SrcType& str) {
  if (str.empty()) {
    return DstType();
  }
  return DstType(str.data(), str.size());
}

template <typename DstType>
RE_FINLINE DstType Convert(stl::wstring str) {
  return stringCopyTemp<stl::wstring, DstType>(str);
}
template <typename DstType>
RE_FINLINE DstType Convert(stl::string str) {
  return stringCopyTemp<stl::string, DstType>(str);
}
template <typename DstType>
RE_FINLINE DstType Convert(string str) {
  return stringCopyTemp<string, DstType>(str);
}
}  // namespace re::engine::container