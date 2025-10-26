#pragma once

#include <string>

#include "CompileDefine.h"
#include "Assert.h"

#include "Object/RObject.h"

namespace RE::Engine {

enum struct ResultType : uint32_t {
  None = 0,
  RE = 1,
  WIN_HRESULT = 2,
};

class RE_DECLSPEC Result : public RObject {
 public:
  Result();
  Result(ResultType type, long code);

  RE_INLINE void SetResultType(ResultType type) { m_type = type; }
  RE_INLINE void SetReusltCode(long code) { m_code = code; }

  RE_INLINE ResultType GetResultType() const { return m_type; }
  RE_INLINE long GetReusltCode() const { return m_code; }

  bool Success() const;
  bool Failed() const;

 private:
  long m_code;
  ResultType m_type;
};

#define REResult(code) Result(ResultType::RE, code)
#define REWinResult(code) Result(ResultType::WIN_HRESULT, code)

#ifdef RE_DEBUG
#define REResultSuccess(code) REAssertCodeRes(REResult(code).Success(), #code, REResult(code).GetReusltCode())
#define REWinResultSuccess(code) REAssertCodeRes(REWinResult(code).Success(), #code, REResult(code).GetReusltCode())
#else
#define REResultSuccess(code) code
#define REWinResultSuccess(code) code
#endif

}  // namespace RE::Engine