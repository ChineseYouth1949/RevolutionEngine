#pragma once

#ifdef RE_DEBUG
#define RE_SET_D3D12_OBJECT_NAEM(pObject, name) \
  if (pObject) {                                \
    (pObject)->SetName(name);                   \
  }
#else
#define RE_SET_D3D12_OBJECT_NAEM(pObject, name)
#endif