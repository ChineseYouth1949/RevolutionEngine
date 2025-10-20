#pragma once

#include <string>
#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace RE::Engine {

class JsonDoc {
 public:
  JsonDoc();
  ~JsonDoc() = default;

  bool Parse(const std::string& jsonStr);
  std::string Stringify() const;

  bool SaveToFile(const std::string& filename) const;
  bool LoadFromFile(const std::string& filename);

  bool HasKey(const std::string& key) const;

  bool GetValue(const std::string& key, std::string& res) const;
  bool GetValue(const std::string& key, int& res) const;
  bool GetValue(const std::string& key, double& res) const;

  bool SetValue(const std::string& key, const std::string& value);
  bool SetValue(const std::string& key, int value);
  bool SetValue(const std::string& key, double value);

  bool Remove(const std::string& key);

 private:
  rapidjson::Document m_doc;
};

}  // namespace RE::Engine