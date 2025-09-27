#pragma once

#ifdef RE_EXPORT
#define RE_API __declspec(dllexport)
#else
#define RE_API __declspec(dllimport)
#endif

#ifdef _MSC_VER
#define INLINE __forceinline
#else
#define INLINE inline __attribute__((always_inline))
#endif