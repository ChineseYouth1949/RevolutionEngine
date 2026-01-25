#pragma once

#include <cstddef>
#include <random>

#include <REngine/Core/Memory/Allocator.h>

using namespace RE::Core::memory;

// The number of benchmark tests within each range
const size_t TestNumBase = 10;

constexpr const size_t SMALL_MIN = 1;
constexpr const size_t SMALL_MAX = 128;

constexpr const size_t MEDIUM_MIN = 129;
constexpr const size_t MEDIUM_MAX = 4096;

constexpr const size_t LARGE_MIN = 4097;
constexpr const size_t LARGE_MAX = 1048576;

constexpr const size_t NEAR_POWER_1 = 1023;
constexpr const size_t NEAR_POWER_2 = 1025;

constexpr const size_t UNALIGNED_1 = 7;
constexpr const size_t UNALIGNED_2 = 15;
constexpr const size_t UNALIGNED_3 = 31;

size_t getRandSize(size_t min, size_t max) {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  return std::uniform_int_distribution<size_t>(min, max)(gen);
}

bool CheckPtrMemoryValid(void* ptr, size_t size) {
  unsigned char* byte_ptr = static_cast<unsigned char*>(ptr);

  try {
    for (size_t i = 0; i < size; ++i) {
      byte_ptr[i] = 'a';
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return false;
  }

  return true;
}

bool CheckPtrMemoryZero(void* ptr, size_t size) {
  unsigned char* byte_ptr = static_cast<unsigned char*>(ptr);
  for (size_t i = 0; i < size; ++i) {
    if (byte_ptr[i] != 0) {
      return false;
    }
  }

  return true;
}