#pragma once

#include "Base/CompileDefine.h"

#include <cinttypes>
#include <typeindex>
#include <atomic>
#include <vector>

namespace RE::Engine {

class Resouce {
 public:
  using Handle = uint64_t;
  enum struct Type {
    None = 0,
  };

  Resouce(Type type) : mType(type), mHandle(sHandle++) {};
  virtual ~Resouce();

  Type GetType() const { return mType; }
  Handle GetHandle() const { return mHandle; }

  virtual bool Build() = 0;
  virtual bool UnBuild() = 0;
  virtual bool IsBuild() = 0;

  std::vector<Resouce*>& GetParCom();
  const std::vector<Resouce*>& GetParCom() const;
  void SetParCom(const std::vector<Resouce*>& parCom);

  std::vector<Resouce*>& GetSubCom();
  const std::vector<Resouce*>& GetSubCom() const;
  void SetSubCom(const std::vector<Resouce*>& subCom);

 protected:
  std::vector<Resouce*> mParCom;
  std::vector<Resouce*> mSubCom;

 private:
  const Type mType;
  const Handle mHandle;

  inline static Handle sHandle = 0;
};

}  // namespace RE::Engine