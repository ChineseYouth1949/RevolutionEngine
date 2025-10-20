#include "Scene/Texture.h"

namespace RE::Engine {

Texture::Texture() {}

Texture::Texture(const std::string& fileName) : m_name(fileName), m_fileName(fileName) {}

}  // namespace RE::Engine