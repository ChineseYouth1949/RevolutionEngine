#pragma once

#include "PCH.h"

#include "Memory/Allocator.h"
#include "Memory/StlAllocator.h"
#include "Memory/Mimalloc.h"

#include "Utility/Assert.h"
#include "Utility/Timer.h"

namespace re::engine {
// Global Dynamic memory allocation
const memory::AllocType GlobalAllocType = memory::AllocType::MiMalloc;
using GAlloc = memory::Allocator<GlobalAllocType>;

template <typename T>
using shared_ptr = memory::Allocator<GlobalAllocType>::shared_ptr<T>;

template <typename T>
using unique_ptr = memory::Allocator<GlobalAllocType>::unique_ptr<T>;

// Global stl continaner
const memory::AllocType GlobalAllocTypeSTL = memory::AllocType::MiMalloc;
template <typename T>
using GAllocSTL = memory::StlAllocator<GlobalAllocTypeSTL, T>;

template <typename T>
using vector = stl::vector<T, GAllocSTL<T>>;

template <typename T>
using deque = stl::deque<T, GAllocSTL<T>>;

template <typename T>
using list = stl::list<T, GAllocSTL<T>>;

template <typename T, size_t N>
using array = stl::array<T, N>;

template <typename T>
using set = stl::set<T, stl::less<T>, GAllocSTL<T>>;

template <typename Key, typename Value>
using map = stl::map<Key, Value, stl::less<Key>, GAllocSTL<stl::pair<const Key, Value>>>;

template <typename T>
using multiset = stl::multiset<T, stl::less<T>, GAllocSTL<T>>;

template <typename T>
using hash_set = stl::unordered_set<T, stl::hash<T>, stl::equal_to<T>, GAllocSTL<T>>;

template <typename Key, typename Value>
using hash_map = stl::unordered_map<Key, Value, stl::hash<Key>, stl::equal_to<Key>, GAllocSTL<stl::pair<const Key, Value>>>;

template <typename Key, typename Value>
using unordered_map = hash_map<Key, Value>;

template <typename T>
using queue = stl::queue<T, deque<T>>;

template <typename T>
using stack = stl::stack<T, deque<T>>;

template <typename T, typename Container = vector<T>, typename Compare = stl::less<typename Container::value_type>>
using priority_queue = stl::priority_queue<T, Container, Compare>;

template <typename T>
using basic_string = stl::basic_string<T, GAllocSTL<T>>;

using string = basic_string<char>;
using wstring = basic_string<wchar_t>;

using u8string = basic_string<char8_t>;
using u16string = basic_string<char16_t>;

}  // namespace re::engine