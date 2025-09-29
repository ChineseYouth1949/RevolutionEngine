#pragma once

#include <string>

#include "CompileDefine.h"

namespace RE::Core {

enum struct ResultType : uint32_t {
  None = 0,
  RE = 1,
  WIN_HRESULT = 2,
};

class RE_API Result {
 public:
  Result();
  Result(ResultType type, long code);

  RE_INLINE void SetResultType(ResultType type) { m_type = type; }
  RE_INLINE void SetReusltCode(long code) { m_code = code; }

  RE_INLINE ResultType GetResultType() const { return m_type; }
  RE_INLINE long GetReusltCode() const { return m_code; }

  bool Success() const;
  bool Failed() const;
  void ThrowIfFailed(const std::string& errInfo) const;

 private:
  long m_code;
  ResultType m_type;
};

#define RESULT_RE(code) Result(ResultType::RE, code)
#define RESULT_WIN_RESULT(code) Result(ResultType::WIN_HRESULT, code)

#define ThrowIfFailed_RE(code) RESULT_RE(code).ThrowIfFailed()
#define ThrowIfFailed_WIN_RESULT(code) RESULT_WIN_RESULT(code).ThrowIfFailed()

}  // namespace RE::Core