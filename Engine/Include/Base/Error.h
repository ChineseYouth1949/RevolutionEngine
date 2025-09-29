#pragma once

#include <stdexcept>

#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#endif

#include "CompileDefine.h"
#include "Result.h"

namespace RE::Core {

class RE_API ResultException : public std::runtime_error {
 public:
  ResultException(const Result& result, const std::string& errInfo) : std::runtime_error(errInfo), m_result(result) {}
  RE_INLINE Result GetResult() const { return m_result; }

 private:
  const Result m_result;
};
}  // namespace RE::Core