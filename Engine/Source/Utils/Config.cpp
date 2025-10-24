#include "Utils/Config.h"

namespace RE::Engine {

Config::Config() {}
Config::~Config() {}

bool Config::SaveToFile(const std::string& filename) const {
  return mDoc.SaveToFile(filename);
}
bool Config::LoadFromFile(const std::string& filename) {
  return mDoc.LoadFromFile(filename);
}

bool Config::HasOption(const std::string& optionName) const {
  return mDoc.HasKey(optionName);
}

bool Config::SetOption(const std::string& optionName, const std::string& value) {
  return mDoc.SetValue(optionName, value);
}
bool Config::SetOption(const std::string& optionName, int value) {
  return mDoc.SetValue(optionName, value);
}
bool Config::SetOption(const std::string& optionName, double value) {
  return mDoc.SetValue(optionName, value);
}

bool Config::GetOption(const std::string& optionName, std::string& res) const {
  return mDoc.GetValue(optionName, res);
}
bool Config::GetOption(const std::string& optionName, int& res) const {
  return mDoc.GetValue(optionName, res);
}
bool Config::GetOption(const std::string& optionName, double& res) const {
  return mDoc.GetValue(optionName, res);
}

bool Config::RemoveOption(const std::string& optionName) {
  return mDoc.Remove(optionName);
}

}  // namespace RE::Engine