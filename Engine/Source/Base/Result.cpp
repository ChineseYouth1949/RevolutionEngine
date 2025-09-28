#include "Base/Result.h"

#include "Base/Error.h"

namespace RE::Core {

Result::Result() : m_code(0), m_type(ResultType::None) {}
Result::Result(ResultType type, long code) : m_code(code), m_type(type) {}

bool Result::Success() const {
  if (m_type == ResultType::None) {
    return false;
  }

  if (m_type == ResultType::RE) {
    return m_code >= 0;
  }

  if (m_type == ResultType::WIN_HRESULT) {
    return SUCCEEDED(m_code);
  }

  return false;
}
bool Result::Failed() const {
  return Success() == false;
}

void Result::ThrowIfFailed(const std::string& errInfo) const {
  if (Failed()) {
    throw ResultException(*this, errInfo);
  }
}

}  // namespace RE::Core