#pragma once

#ifdef RE_EXPORT
#define RE_API __declspec(dllexport)
#else
#define RE_API __declspec(dllimport)
#endif