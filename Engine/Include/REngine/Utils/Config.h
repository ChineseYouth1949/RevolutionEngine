#pragma once

#include "Base/CompileDefine.h"
#include "Base/Result.h"

#include "Object/RObject.h"

#include "Json.h"

namespace RE::Engine {

class RE_DECLSPEC Config {
 public:
  Config();
  ~Config();

  bool SaveToFile(const std::string& filename) const;
  bool LoadFromFile(const std::string& filename);

  bool HasOption(const std::string& optionName) const;

  bool SetOption(const std::string& optionName, const std::string& value);
  bool SetOption(const std::string& optionName, int value);
  bool SetOption(const std::string& optionName, double value);

  bool GetOption(const std::string& optionName, std::string& res) const;
  bool GetOption(const std::string& optionName, int& res) const;
  bool GetOption(const std::string& optionName, double& res) const;

  bool RemoveOption(const std::string& optionName);

 private:
  JsonDoc mDoc;
};

}  // namespace RE::Engine