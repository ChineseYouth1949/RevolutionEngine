#pragma once

#include "Base/CompileDefine.h"

#include <cinttypes>
#include <typeindex>
#include <atomic>
#include <vector>

namespace RE::Engine {

class Component {
 public:
  using Handle = uint64_t;
  using Type = std::type_index;

  Component(Type type) : mType(type), mHandle(sHandle++) {};
  virtual ~Component();

  Type GetType() const { return mType; }
  Handle GetHandle() const { return mHandle; }

  virtual bool Build() = 0;
  virtual bool UnBuild() = 0;
  virtual bool IsBuild() = 0;

  std::vector<Component*>& GetParCom();
  const std::vector<Component*>& GetParCom() const;
  void SetParCom(const std::vector<Component*>& parCom);

  std::vector<Component*>& GetSubCom();
  const std::vector<Component*>& GetSubCom() const;
  void SetSubCom(const std::vector<Component*>& subCom);

 protected:
  std::vector<Component*> mParCom;
  std::vector<Component*> mSubCom;

 private:
  const Type mType;
  const Handle mHandle;

  inline static Handle sHandle = 0;
};

}  // namespace RE::Engine