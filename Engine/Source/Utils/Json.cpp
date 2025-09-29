
#include "Utils/Json.h"

#include <fstream>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace RE::Core {

struct JsonDoc::Impl {
  rapidjson::Document doc;
};

JsonDoc::JsonDoc() {
  m_impl->doc.SetObject();
}

bool JsonDoc::Parse(const std::string& jsonStr) {
  m_impl->doc.Parse(jsonStr.c_str());
  return !m_impl->doc.HasParseError();
}

std::string JsonDoc::Stringify() const {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  m_impl->doc.Accept(writer);
  return buffer.GetString();
}

bool JsonDB::ExportToFile(const JsonDoc& doc, const std::string& filename) {
  std::ofstream out(filename);
  if (!out.is_open())
    return false;
  out << doc.Stringify();
  out.close();
  return true;
}

bool JsonDB::ImportFromFile(JsonDoc& doc, const std::string& filename) {
  std::ifstream in(filename);
  if (!in.is_open())
    return false;
  std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  return doc.Parse(content);
}

}  // namespace RE::Core