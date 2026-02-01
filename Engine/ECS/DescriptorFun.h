#pragma once

#include "Engine/Base/All.h"

namespace re::engine::ecs {
class Resocue;

template <typename Type>
RE_FINLINE static const utility::Descriptor& GetResDescriptor() {
  return utility::DescriptorFactory::GetAt<Resource, Type>();
}

struct Component {};

template <typename Type>
RE_FINLINE static const utility::Descriptor& GetComDescriptor() {
  return utility::DescriptorFactory::GetAt<Component, Type>();
}

}  // namespace re::engine::ecs