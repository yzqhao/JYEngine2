#include "LuaVirtualMachine.h"
#include "Core/Interface/ILogSystem.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/IScriptSystem.h"
#include "Engine/Engine.h"
#include "../LuaScriptSystem.h"
#include "../BindingMath.h"
#include "../BindingCore.h"
#include "../BindingEngine.h"
#include "Function/imguifunction/AdapterImgui.h"
//#include "../BindingImgui.h"

NS_JYE_BEGIN

static std::map<lua_State*, sol::state*> s_mapLuaState;

// Use raw function of form "int(lua_State*)"
// -- this is called a "raw C function",
// and matches the type for lua_CFunction
int LoadFileRequire(lua_State* L) 
{
	// use sol2 stack API to pull
	// "first argument"
	std::string path = sol::stack::get<std::string>(L, 1);

	std::map<std::string, std::function<void(sol::state&)>> allRegisterLib =
	{
		{ "Math", Bind_Math },
		{ "Core", Bind_Core },
		{ "Engine", [](sol::state& state) { Bind_Engine(state); Bind_Engine_Entity(state); } },
#ifdef _EDITOR
		{ "imguifunction", [](sol::state& state) { 
				Engine::Instance()->LoadModule("imguifunction");
				//Bind_Imgui(state);
				AdapterImgui::Instance()->LoadImgui(state);
			} 
		},
#endif
	};

	auto modulename = allRegisterLib.find(path);
	if (modulename != allRegisterLib.end())
	{
		modulename->second(*s_mapLuaState[L]);
		std::string script = "return " + path;
		luaL_loadbuffer(L, script.data(), script.size(), path.c_str());
		return 1;
	}

	sol::stack::push(L, "This is not the module you're looking for!");
	return 1;
}

LuaVirtualMachine::LuaVirtualMachine(uint index)
	: IVirtualMachine(index)
{
	m_solState.open_libraries(sol::lib::base,
		sol::lib::package,
		sol::lib::coroutine,
		sol::lib::string,
		sol::lib::os,
		sol::lib::math,
		sol::lib::table,
		sol::lib::debug,
		sol::lib::bit32,
		sol::lib::io,
		sol::lib::utf8
	);

	m_solState.add_package_loader(LoadFileRequire);

	s_mapLuaState.insert({ m_solState.lua_state(), &m_solState });
}

LuaVirtualMachine::~LuaVirtualMachine()
{
	s_mapLuaState.erase(m_solState.lua_state());
}

void LuaVirtualMachine::Initialize()
{
	//ÉèÖÃluaËÑË÷Ä¿Â¼
	sol::table package_table = m_solState["package"];
	std::string path = package_table["path"];
	path.append(IScriptSystem::Instance()->GetPackagePath());
	package_table["path"] = path;

#ifdef _DEBUG
	m_solState.set("_DEBUG", true);
#endif // _DEBUG
#if _EDITOR
	m_solState.set("_EDITOR", true);
#endif
#if _PLATFORM_WINDOWS
	m_solState.set("_PLATFORM_WINDOWS", true);
#endif
}

void LuaVirtualMachine::Exit()
{
}

void LuaVirtualMachine::CallScript(const std::string& script_file_path)
{
	auto result = m_solState.script_file(IFileSystem::Instance()->PathAssembly(script_file_path));
	if (result.valid() == false)
	{
		sol::error err = result;
		JYERROR("---- LOAD LUA ERROR ----%s------------------------", err.what());
	}
}

void LuaVirtualMachine::DoLoadScript(const std::string& script_file_path)
{
	auto result = m_solState.script_file(IFileSystem::Instance()->PathAssembly(script_file_path));
	if (result.valid() == false)
	{
		sol::error err = result;
		JYERROR("---- LOAD LUA ERROR ----%s------------------------", err.what());
		return ;
	}
	int returncount = result.return_count();
	JY_ASSERT(returncount == 1);
	sol::object obj = result.get<sol::object>(0);
	sol::table luatable = obj.as<sol::table>();

	//luatable[sol::meta_function::index] = luatable;
	//m_res = m_solState.create_table_with();
	//m_res[sol::metatable_key] = luatable;

	m_res = luatable;
}

void LuaVirtualMachine::CallScriptFunc(const std::string& function_name)
{
	sol::protected_function main_function = m_solState[function_name];
	sol::protected_function_result result = main_function();
	if (result.valid() == false)
	{
		sol::error err = result;
		JYERROR("---- RUN LUA_FUNCTION ERROR ----%s------------------------", err.what());
	}
}

void* LuaVirtualMachine::GetResult()
{
	return &m_res;
}

NS_JYE_END
