#pragma once

#include <string>

namespace RE::Engine {

class Texture {
 public:
  Texture();
  Texture(const std::string& fileName);

  std::string m_name;
  std::string m_fileName;
};

}  // namespace RE::Engine