#pragma once

#include "Base/Base.h"

namespace RE::Core {

class JsonDoc {
 public:
  JsonDoc();
  ~JsonDoc();

  bool Parse(const std::string& jsonStr);
  std::string Stringify() const;

  bool HasKey(const std::string& key);

  std::string GetString(const std::string& key);
  int GetInt(const std::string& key);

  bool SetValue(const std::string& key, const std::string& value);
  bool RemoveKey(const std::string& key);

 private:
  RE_PIMPL
};

class JsonDB {
 public:
  static bool ExportToFile(const JsonDoc& doc, const std::string& filename);
  static bool ImportFromFile(JsonDoc& doc, const std::string& filename);
};

}  // namespace RE::Core