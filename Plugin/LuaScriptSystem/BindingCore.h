#pragma once

#include "Math/3DMath.h"

#include <sol/sol.hpp>

NS_JYE_BEGIN

template<typename T>
class Lua_Int64
{
public:
	Lua_Int64(T val = 0) : m_int64(val) {}

	const T& GetVal() { return m_int64; }

	bool operator == (const Lua_Int64& v) const { return m_int64 == v.m_int64; }
	bool operator != (const Lua_Int64& v) const { return m_int64 != v.m_int64; }

	Lua_Int64 operator - () const { return Lua_Int64(-m_int64); }

	Lua_Int64 operator + (const Lua_Int64& v) const { return Lua_Int64(m_int64 + v.m_int64); }
	Lua_Int64 operator - (const Lua_Int64& v) const { return Lua_Int64(m_int64 - v.m_int64); }
	Lua_Int64 operator * (Lua_Int64 k) const { return Lua_Int64(m_int64 * k.m_int64); }
	Lua_Int64 operator / (Lua_Int64 k) const { return Lua_Int64(m_int64 / k.m_int64); }

	std::string toString() const {
		char tempBuffer[256];
		sprintf(tempBuffer, "%lld", m_int64);
		return std::string(tempBuffer);
	}

private:
	T m_int64;
};
typedef Lua_Int64<int64> LuaInt64;
typedef Lua_Int64<uint64> LuaUInt64;

#define REGISTER_OBJECT_FUNC(obj) \
	"RTTI", & obj::RTTI	

void Bind_Core(sol::state& sol_state);

NS_JYE_END

