#pragma once

#include <stdexcept>

#include "CompileDefine.h"
#include "Result.h"

namespace RE::Engine {

class RE_API ResultException : public std::runtime_error {
 public:
  ResultException(const Result& result, const std::string& errInfo) : std::runtime_error(errInfo), m_result(result) {}
  RE_INLINE Result GetResult() const { return m_result; }

 private:
  const Result m_result;
};
}  // namespace RE::Engine