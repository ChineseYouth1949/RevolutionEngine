#pragma once

/**
 * Compilers.
 */
#define RE_COMPILER_MSVC 1
#define RE_COMPILER_CLANG 2
#define RE_COMPILER_GCC 3

#ifndef RE_COMPILER
#if defined(_MSC_VER)
#define RE_COMPILER RE_COMPILER_MSVC
#elif defined(__clang__)
#define RE_COMPILER RE_COMPILER_CLANG
#elif defined(__GNUC__)
#define RE_COMPILER RE_COMPILER_GCC
#else
#error "Unsupported compiler"
#endif
#endif  // RE_COMPILER

#define RE_MSVC (RE_COMPILER == RE_COMPILER_MSVC)
#define RE_CLANG (RE_COMPILER == RE_COMPILER_CLANG)
#define RE_GCC (RE_COMPILER == RE_COMPILER_GCC)

/**
 * Platforms.
 */
#define RE_PLATFORM_WINDOWS 1
#define RE_PLATFORM_LINUX 2

#ifndef RE_PLATFORM
#if defined(_WIN64)
#define RE_PLATFORM RE_PLATFORM_WINDOWS
#elif defined(__linux__)
#define RE_PLATFORM RE_PLATFORM_LINUX
#else
#error "Unsupported target platform"
#endif
#endif  // RE_PLATFORM

#define RE_WINDOWS (RE_PLATFORM == RE_PLATFORM_WINDOWS)
#define RE_LINUX (RE_PLATFORM == RE_PLATFORM_LINUX)

/**
 * D3D12 Agility SDK.
 */
#define RE_D3D12_AGILITY_SDK_PATH ".\\D3D12\\"
// To enable the D3D12 Agility SDK, this macro needs to be added to the main source file of the executable.
#define RE_EXPORT_D3D12_AGILITY_SDK                                                               \
  extern "C" {                                                                                    \
  __declspec(dllexport) extern const unsigned int D3D12SDKVersion = RE_D3D12_AGILITY_SDK_VERSION; \
  }                                                                                               \
  extern "C" {                                                                                    \
  __declspec(dllexport) extern const char* D3D12SDKPath = RE_D3D12_AGILITY_SDK_PATH;              \
  }

/**
 * Shared library (DLL) export and import.
 */
#ifdef RE_EXPORT
#define RE_DECLSPEC __declspec(dllexport)
#else
#define RE_DECLSPEC __declspec(dllimport)
#endif

#define RE_API RE_DECLSPEC

/**
 * force inline.
 */
#if RE_MSVC
#define RE_FINLINE __forceinline
#elif RE_CLANG | RE_GCC
#define RE_FINLINE __attribute__((always_inline))
#endif

/**
 * Preprocessor stringification.
 */
#define RE_STRINGIZE(a) #a
#define RE_CONCAT_STRINGS_(a, b) a##b
#define RE_CONCAT_STRINGS(a, b) RE_CONCAT_STRINGS_(a, b)

/**
 * Implement logical operators on a class enum for making it usable as a flags enum.
 */
// clang-format off
#define RE_ENUM_CLASS_OPERATORS(e_) \
    inline e_ operator& (e_ a, e_ b) { return static_cast<e_>(static_cast<int>(a)& static_cast<int>(b)); } \
    inline e_ operator| (e_ a, e_ b) { return static_cast<e_>(static_cast<int>(a)| static_cast<int>(b)); } \
    inline e_& operator|= (e_& a, e_ b) { a = a | b; return a; }; \
    inline e_& operator&= (e_& a, e_ b) { a = a & b; return a; }; \
    inline e_  operator~ (e_ a) { return static_cast<e_>(~static_cast<int>(a)); } \
    inline bool is_set(e_ val, e_ flag) { return (val & flag) != static_cast<e_>(0); } \
    inline void flip_bit(e_& val, e_ flag) { val = is_set(val, flag) ? (val & (~flag)) : (val | flag); }
// clang-format on

#define RE_UINT32_MAX 0xFFFFFFFFU