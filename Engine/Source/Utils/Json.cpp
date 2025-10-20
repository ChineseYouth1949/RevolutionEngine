#include "Utils/Json.h"

namespace RE::Engine {

JsonDoc::JsonDoc() {
  m_doc.SetObject();
}

bool JsonDoc::Parse(const std::string& jsonStr) {
  return !m_doc.Parse(jsonStr.c_str()).HasParseError();
}

std::string JsonDoc::Stringify() const {
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  m_doc.Accept(writer);
  return buffer.GetString();
}

bool JsonDoc::SaveToFile(const std::string& filename) const {
  std::ofstream ofs(filename);
  if (!ofs.is_open()) {
    return false;
  }
  ofs << Stringify();
  return true;
}

bool JsonDoc::LoadFromFile(const std::string& filename) {
  std::ifstream ifs(filename);
  if (!ifs.is_open()) {
    return false;
  }

  std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
  return Parse(content);
}

bool JsonDoc::HasKey(const std::string& key) const {
  return m_doc.HasMember(key.c_str());
}

bool JsonDoc::GetValue(const std::string& key, std::string& value) const {
  if (!HasKey(key) || !m_doc[key.c_str()].IsString()) {
    return false;
  }
  value = m_doc[key.c_str()].GetString();
  return true;
}

bool JsonDoc::GetValue(const std::string& key, int& value) const {
  if (!HasKey(key) || !m_doc[key.c_str()].IsInt()) {
    return false;
  }
  value = m_doc[key.c_str()].GetInt();
  return true;
}

bool JsonDoc::GetValue(const std::string& key, double& value) const {
  if (!HasKey(key) || !m_doc[key.c_str()].IsDouble()) {
    return false;
  }
  value = m_doc[key.c_str()].GetDouble();
  return true;
}

bool JsonDoc::SetValue(const std::string& key, const std::string& value) {
  rapidjson::Value v(value.c_str(), m_doc.GetAllocator());
  m_doc.AddMember(rapidjson::StringRef(key.c_str()), v, m_doc.GetAllocator());
  return true;
}

bool JsonDoc::SetValue(const std::string& key, int value) {
  rapidjson::Value v(value);
  m_doc.AddMember(rapidjson::StringRef(key.c_str()), v, m_doc.GetAllocator());
  return true;
}

bool JsonDoc::SetValue(const std::string& key, double value) {
  rapidjson::Value v(value);
  m_doc.AddMember(rapidjson::StringRef(key.c_str()), v, m_doc.GetAllocator());
  return true;
}

bool JsonDoc::Remove(const std::string& key) {
  if (!HasKey(key)) {
    return false;
  }
  return m_doc.RemoveMember(key.c_str());
}

}  // namespace RE::Engine
