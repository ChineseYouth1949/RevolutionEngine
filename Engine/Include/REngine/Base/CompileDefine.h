#pragma once

#ifdef RE_EXPORT
#define RE_DECLSPEC __declspec(dllexport)
#else
#define RE_DECLSPEC __declspec(dllimport)
#endif

#ifdef _MSC_VER
#define RE_INLINE __forceinline
#else
#define RE_INLINE inline __attribute__((always_inline))
#endif