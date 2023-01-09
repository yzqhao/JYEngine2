
#include "LuaMaterialParse.h"
#include "lua/lua.hpp"
#include "utils/logutils.h"

#include <cassert>
#include <cassert>

namespace OrangeFilter {

#ifndef OF_NO_LUA
	static void set_global_int(lua_State* L, const char* key, int value)
	{
		lua_pushinteger(L, value);
		lua_setglobal(L, key);
	}

	static void get_table_string(lua_State* L, const char* key, std::string& str)
	{
		lua_pushstring(L, key);
		lua_gettable(L, -2);
		if (lua_isstring(L, -1))
		{
			str = lua_tostring(L, -1);
		}
		lua_pop(L, 1);
	}

	template <class T>
	static void get_table_int(lua_State* L, const char* key, T& num)
	{
		lua_pushstring(L, key);
		lua_gettable(L, -2);
		if (lua_isinteger(L, -1))
		{
			num = (T)lua_tointeger(L, -1);
		}
		lua_pop(L, 1);
	}

	static void add_lua_path(lua_State* L, const std::string& path)
	{
		lua_getglobal(L, "package");
		lua_getfield(L, -1, "path"); // get field "path" from table at top of stack (-1)
		std::string curPath = lua_tostring(L, -1); // grab path string from top of stack
		curPath.append(";"); // do your path magic here
		curPath.append(path);
		lua_pop(L, 1); // get rid of the string on the stack we just pushed on line 5
		lua_pushstring(L, curPath.c_str()); // push the new one
		lua_setfield(L, -2, "path"); // set the field "path" in table at -2 with value at top of stack
		lua_pop(L, 1); // get rid of package table from top of stack
	}

	LuaMaterialParse::LuaMaterialParse()
	{
	}

	LuaMaterialParse::~LuaMaterialParse()
	{
	}

	void LuaMaterialParse::Initialize(lua_State* m_luaState)
	{
#define  MAKE_STRING_ENUM(x,y) #y , (int)x##y
#define SET_GLOBAL_INT(x, y) set_global_int(m_luaState, MAKE_STRING_ENUM(x, y));

		SET_GLOBAL_INT(MPC_, ALPAH_OFF);
		SET_GLOBAL_INT(MPC_, ALPAH_OFF);
		SET_GLOBAL_INT(MPC_, ALPAH_COVERAGE);
		SET_GLOBAL_INT(MPC_, ALPAH_BLEND);
		SET_GLOBAL_INT(MPC_, ZERO);
		SET_GLOBAL_INT(MPC_, ONE);
		SET_GLOBAL_INT(MPC_, SRC_COLOR);
		SET_GLOBAL_INT(MPC_, ONE_MINUS_SRC_COLOR);
		SET_GLOBAL_INT(MPC_, SRC_ALPHA);
		SET_GLOBAL_INT(MPC_, ONE_MINUS_SRC_ALPHA);
		SET_GLOBAL_INT(MPC_, DST_ALPHA);
		SET_GLOBAL_INT(MPC_, ONE_MINUS_DST_ALPHA);
		SET_GLOBAL_INT(MPC_, DST_COLOR);
		SET_GLOBAL_INT(MPC_, ONE_MINUS_DST_COLOR);
		SET_GLOBAL_INT(MPC_, DEPTH_TEST_ON);
		SET_GLOBAL_INT(MPC_, DEPTH_TEST_OFF);
		SET_GLOBAL_INT(MPC_, DEPTH_MASK_ON);
		SET_GLOBAL_INT(MPC_, DEPTH_MASK_OFF);
		SET_GLOBAL_INT(MPC_, DEPTH_FUNCTION_NEVER);
		SET_GLOBAL_INT(MPC_, DEPTH_FUNCTION_LESS);

		SET_GLOBAL_INT(MPC_, DEPTH_FUNCTION_GEQUAL);
		SET_GLOBAL_INT(MPC_, DEPTH_FUNCTION_LEQUAL);
		SET_GLOBAL_INT(MPC_, DEPTH_FUNCTION_GREATER);
		SET_GLOBAL_INT(MPC_, DEPTH_FUNCTION_ALWAYS);
		SET_GLOBAL_INT(MPC_, CULL_FACE_OFF);
		SET_GLOBAL_INT(MPC_, CULL_FACE_BACK);
		SET_GLOBAL_INT(MPC_, CULL_FACE_FRONT);
		SET_GLOBAL_INT(MPC_, CULL_FACE_BACK_CW);
		SET_GLOBAL_INT(MPC_, CULL_FACE_FRONT_CW);

		SET_GLOBAL_INT(MPC_, COLOR_NONE);
		SET_GLOBAL_INT(MPC_, COLOR_RGBA);
		SET_GLOBAL_INT(MPC_, COLOR_RGB);
		SET_GLOBAL_INT(MPC_, STENCIL_ON);
		SET_GLOBAL_INT(MPC_, STENCIL_OFF);
		SET_GLOBAL_INT(MPC_, STENCIL_FUNCTION_NEVER);
		SET_GLOBAL_INT(MPC_, STENCIL_FUNCTION_LESS);
		SET_GLOBAL_INT(MPC_, STENCIL_FUNCTION_EQUAL);
		SET_GLOBAL_INT(MPC_, STENCIL_FUNCTION_LEQUAL);
		SET_GLOBAL_INT(MPC_, STENCIL_FUNCTION_GREATER);
		SET_GLOBAL_INT(MPC_, STENCIL_FUNCTION_NOTEQUAL);
		SET_GLOBAL_INT(MPC_, STENCIL_FUNCTION_GEQUAL);
		SET_GLOBAL_INT(MPC_, STENCIL_FUNCTION_ALWAYS);
		SET_GLOBAL_INT(MPC_, STENCIL_OPERATION_ZERO);
		SET_GLOBAL_INT(MPC_, STENCIL_OPERATION_ONE);
		SET_GLOBAL_INT(MPC_, STENCIL_OPERATION_KEEP);
		SET_GLOBAL_INT(MPC_, STENCIL_OPERATION_REPLACE);
		SET_GLOBAL_INT(MPC_, STENCIL_OPERATION_INCR);
		SET_GLOBAL_INT(MPC_, STENCIL_OPERATION_DECR);
		SET_GLOBAL_INT(MPC_, STENCIL_OPERATION_INVERT);
		SET_GLOBAL_INT(MPC_, STENCIL_OPERATION_INCR_WRAP);
		SET_GLOBAL_INT(MPC_, STENCIL_OPERATION_DECR_WRAP);
		SET_GLOBAL_INT(MPC_, POLYGON_POINT);
		SET_GLOBAL_INT(MPC_, POLYGON_LINE);
		SET_GLOBAL_INT(MPC_, POLYGON_TRIANGLE);

		SET_GLOBAL_INT(ParserMaterial::VT_, FLOAT);
		SET_GLOBAL_INT(ParserMaterial::VT_, FLOATRANGE);
		SET_GLOBAL_INT(ParserMaterial::VT_, VEC2);
		SET_GLOBAL_INT(ParserMaterial::VT_, VEC3);
		SET_GLOBAL_INT(ParserMaterial::VT_, VEC4);
		SET_GLOBAL_INT(ParserMaterial::VT_, COLOR);
		SET_GLOBAL_INT(ParserMaterial::VT_, TEXTURE1D);
		SET_GLOBAL_INT(ParserMaterial::VT_, TEXTURE2D);
		SET_GLOBAL_INT(ParserMaterial::VT_, TEXTURE3D);
		SET_GLOBAL_INT(ParserMaterial::VT_, TEXTURECUBE);
		SET_GLOBAL_INT(ParserMaterial::VT_, KEYWORDENUM);

#undef SET_GLOBAL_INT
#undef MAKE_STRING_ENUM
	}

	bool LuaMaterialParse::ParseMaterial(const std::string& path, ParserMaterial& matData)
	{
		lua_State* m_luaState = luaL_newstate();
		luaL_openlibs(m_luaState);

		Initialize(m_luaState);

		if (luaL_dofile(m_luaState, path.c_str()) != 0)
		{
			const char* error = lua_tostring(m_luaState, -1);
			lua_pop(m_luaState, 1);
			OF_LOGE("lua error: %s\n", error);
			return false;
		}

		// TODO

		_ParsePropertys(matData);
		_ParseRenderQueue(matData);

		lua_close(m_luaState);

		return true;
	}

	void LuaMaterialParse::_ParsePropertys(ParserMaterial& matData)
	{

	}

	void LuaMaterialParse::_ParseRenderQueue(ParserMaterial& matData)
	{

	}
#else
	bool LuaMaterialParse::ParseMaterial(const std::string& path, const std::string& shaderApi, ParserMaterial& matData) { return false; }
	void LuaMaterialParse::_ParsePropertys(ParserMaterial& matData) { }
	void LuaMaterialParse::_ParseRenderQueue(ParserMaterial& matData) { }
#endif
}