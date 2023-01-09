
#pragma once

#include "System.h"
#include <stdarg.h>

NS_JYE_BEGIN

template<class T>
using Vector = std::vector<T>;

template<class T, int N>
using Array = std::array<T, N>;

template<class T>
using List = std::list<T>;

template<class T>
using Queue = std::queue<T>;

template<class T>
using Stack = std::stack<T>;

template<class T, class U>
using Map = std::map<T, U>;

template<class T, class U>
using HashMap = std::unordered_map<T, U>;

template<class T, class U, class K>
using HashMapKey = std::unordered_map<T, U, K>;

template<class T>
using Set = std::set<T>;

template<class T>
using HashSet = std::unordered_set<T>;

using String = std::string;

template<class T>
SYSTEM_API bool IsVectorIndexValid(const Vector<T>& vec, int idx);

SYSTEM_API String StringFormat(const char* format, ...);

extern SYSTEM_API String NAME_None;

NS_JYE_END
