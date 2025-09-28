#pragma once

#include "stdafx.h"

#include <stdexcept>

#include <filesystem>
#include <fstream>
#include <sstream>

#include <pix.h>

using Microsoft::WRL::ComPtr;

inline std::string HrToString(HRESULT hr) {
  std::stringstream ss;
  ss << "HRESULT of 0x" << std::hex << std::setw(8) << std::setfill('0') << static_cast<UINT>(hr);
  return ss.str();
}

class HrException : public std::runtime_error {
 public:
  HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), m_hr(hr) {}
  HRESULT Error() const { return m_hr; }

 private:
  const HRESULT m_hr;
};

#define SAFE_RELEASE(p) \
  if (p) {              \
    (p)->Release();     \
  }

inline void ThrowIfFailed(HRESULT hr) {
  if (FAILED(hr)) {
    throw HrException(hr);
  }
}

const unsigned long PATH_MAX_LENGTH = 1000;

inline std::wstring GetAppPath() {
  wchar_t modulePath[PATH_MAX_LENGTH] = {0};
  auto size = GetModuleFileNameW(nullptr, modulePath, PATH_MAX_LENGTH);

  if (size == 0 || size == PATH_MAX_LENGTH) {
    throw std::runtime_error("Failed to get module path");
  }

  std::filesystem::path path(modulePath);
  std::wstring appPathStr = path.parent_path().wstring() + L'\\';

  return appPathStr;
}

inline std::wstring GetAppParentPath() {
  std::filesystem::path appPath(GetAppPath());
  std::wstring appParentPathStr = appPath.parent_path().wstring() + L'\\';
  return appParentPathStr;
}

inline std::wstring GetAppParentRelativePath(const std::wstring& relaPath) {
  return GetAppParentPath() + relaPath;
}

inline std::vector<uint8_t> ReadDataFromFile(const std::wstring& fileName) {
  const std::filesystem::path& filePath(fileName);

  if (!std::filesystem::exists(filePath)) {
    throw std::system_error(ERROR_FILE_NOT_FOUND, std::system_category(), "File not found: " + filePath.string());
  }

  const auto file_size = std::filesystem::file_size(filePath);
  if (file_size > SIZE_MAX) {
    throw std::runtime_error("File too large");
  }

  std::ifstream file(filePath, std::ios::binary);
  if (!file) {
    throw std::system_error(errno, std::system_category(), "Failed to open file");
  }

  std::vector<uint8_t> buffer(file_size);
  if (!file.read(reinterpret_cast<char*>(buffer.data()), buffer.size())) {
    throw std::system_error(errno, std::system_category(), "Failed to read file");
  }

  return buffer;
}

struct DDS_PIXELFORMAT {
  UINT size;
  UINT flags;
  UINT fourCC;
  UINT rgbBitCount;
  UINT rBitMask;
  UINT gBitMask;
  UINT bBitMask;
  UINT aBitMask;
};

struct DDS_HEADER {
  UINT size;
  UINT flags;
  UINT height;
  UINT width;
  UINT pitchOrLinearSize;
  UINT depth;
  UINT mipMapCount;
  UINT reserved1[11];
  DDS_PIXELFORMAT ddsPixelFormat;
  UINT caps;
  UINT caps2;
  UINT caps3;
  UINT caps4;
  UINT reserved2;
};

inline std::vector<uint8_t> ReadDDSData(std::wstring fileName) {
  std::vector<uint8_t> buffer = ReadDataFromFile(fileName);

  constexpr UINT DDS_MAGIC = 0x20534444;
  if (buffer.size() < sizeof(UINT) || *reinterpret_cast<const UINT*>(buffer.data()) != DDS_MAGIC) {
    throw std::runtime_error("Invalid DDS file format");
  }

  if (buffer.size() < sizeof(UINT) + sizeof(DDS_HEADER)) {
    throw std::runtime_error("DDS file too small for header");
  }

  const auto header = reinterpret_cast<const DDS_HEADER*>(buffer.data() + sizeof(UINT));

  if (header->size != sizeof(DDS_HEADER) || header->ddsPixelFormat.size != sizeof(DDS_PIXELFORMAT)) {
    throw std::runtime_error("Invalid DDS header structure");
  }

  const size_t dataOffset = sizeof(UINT) + sizeof(DDS_HEADER);
  return std::vector<uint8_t>(buffer.begin() + dataOffset, buffer.end());
}

#if defined(_DEBUG) || defined(DBG)
inline void SetName(ID3D12Object* pObject, const std::wstring& name) {
  if (pObject && !name.empty()) {
    pObject->SetName(name.c_str());
  }
}

inline void SetNameIndexed(ID3D12Object* pObject, const std::wstring& name, uint32_t index) {
  if (pObject && !name.empty()) {
    std::wstring fullName = name + L"[" + std::to_wstring(index) + L"]";
    pObject->SetName(fullName.c_str());
  }
}
#else
inline void SetName(ID3D12Object*, const std::wstring&) noexcept {}
inline void SetNameIndexed(ID3D12Object*, const std::wstring&, uint32_t) noexcept {}
#endif

#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

inline UINT CalculateConstantBufferByteSize(UINT byteSize) {
  return (byteSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
}

#ifdef D3D_COMPILE_STANDARD_FILE_INCLUDE
inline Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring& filename, const D3D_SHADER_MACRO* defines, const std::string& entrypoint,
                                                      const std::string& target) {
  UINT compileFlags = 0;
#if defined(_DEBUG) || defined(DBG)
  compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

  HRESULT hr;

  Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
  Microsoft::WRL::ComPtr<ID3DBlob> errors;
  hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entrypoint.c_str(), target.c_str(), compileFlags, 0,
                          &byteCode, &errors);

  if (errors != nullptr) {
    OutputDebugStringA((char*)errors->GetBufferPointer());
  }
  ThrowIfFailed(hr);

  return byteCode;
}
#endif

template <class T>
void ResetComPtrArray(T* comPtrArray) {
  for (auto& i : *comPtrArray) {
    i.Reset();
  }
}

template <class T>
void ResetUniquePtrArray(T* uniquePtrArray) {
  for (auto& i : *uniquePtrArray) {
    i.reset();
  }
}