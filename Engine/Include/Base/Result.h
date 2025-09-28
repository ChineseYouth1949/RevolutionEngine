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

#define ThrowIfFailed_RE(x) Result(ResultType::RE, x).ThrowIfFailed()
#define ThrowIfFailed_WIN_HRESULT(x) Result(ResultType::WIN_HRESULT, x).ThrowIfFailed()

}  // namespace RE::Core