#pragma once

#include "Engine/Base/PCH.h"
#include "Container.h"

namespace re::engine::utility {

struct Descriptor {
  Descriptor() = default;

  RE_FINLINE uint32_t Id() const { return id; }
  RE_FINLINE uint32_t Tag() const { return tag; }
  RE_FINLINE uint64_t Hash() const { return (static_cast<uint64_t>(tag) << 32) | id; }
  RE_FINLINE const string& Descrip() const { return descrip; }

  RE_FINLINE bool Valid() const { return id != 0; }
  RE_FINLINE bool operator==(const Descriptor& other) const { return id == other.id && tag == other.tag; }

 private:
  Descriptor(size_t _id, uint32_t _tag = 0, const string& _descrip = "") : id(_id), tag(_tag), descrip(_descrip) {}

  uint32_t id{0};
  uint32_t tag{0};
  string descrip;

  friend class DescriptorFactory;
};

class RE_API DescriptorFactory {
 public:
  static inline Descriptor Null = Descriptor(0);

  RE_FINLINE static Descriptor Create(const string& name = "") {
    size_t id = sIdCount.fetch_add(1);
    return {id, 0, name.empty() ? string("DynamicID_") + Convert<string>(stl::to_string(id)) : name};
  }

  template <typename Type>
  RE_FINLINE static const Descriptor& GetAt() {
    static const Descriptor handle = []() {
      Descriptor desc;
      desc.id = sIdCount.fetch_add(1);
      desc.descrip = string(typeid(Type).name()) + string("_") + Convert<string>(stl::to_string(desc.id));
      return desc;
    }();
    return handle;
  }

  template <typename TagClass, typename Type>
  RE_FINLINE static const Descriptor& GetAt() {
    static const Descriptor handle = []() {
      Descriptor desc;
      desc.id = sIdCount.fetch_add(1);
      desc.tag = GetClassTag<TagClass>();
      desc.descrip =
          string(typeid(TagClass).name()) + string("_") + string(typeid(Type).name()) + string("_") + Convert<string>(stl::to_string(desc.id));
      return desc;
    }();
    return handle;
  }

 private:
  template <typename Class>
  RE_FINLINE static uint32_t GetClassTag() {
    static const uint32_t tag = sTagClassCounter.fetch_add(1);
    return tag;
  };

  static inline std::atomic<size_t> sIdCount{1};
  static inline std::atomic<uint32_t> sTagClassCounter{1};
};

}  // namespace re::engine::utility