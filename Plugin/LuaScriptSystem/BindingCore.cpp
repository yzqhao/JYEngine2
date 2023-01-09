
#include "BindingCore.h"

#include "Core/Object.h"
#include "Core/Rtti.h"
#include "Core/Interface/IApplication.h"
#include "Core/Interface/IFile.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/ILogSystem.h"
#include "Core/Interface/IResourceSystem.h"
#include "Core/Interface/IScriptSystem.h"
#include "Core/Interface/ITimeSystem.h"
#include "Core/Interface/IWindow.h"
#include "Core/Object.h"
#include "Core/Rtti.h"

#include <sol/sol.hpp>

NS_JYE_BEGIN

void Bind_Core(sol::state& sol_state)
{
	auto ns_table = sol_state["Core"].get_or_create<sol::table>();

	{
		ns_table.new_usertype<LuaInt64>("Int64", sol::call_constructor, sol::constructors<
			sol::types<>,
			sol::types<int64>>(),
			sol::meta_function::to_string, [](const LuaInt64* vec) -> std::string { return vec->toString(); },
			sol::meta_function::addition, [](const LuaInt64* vec_a, const LuaInt64* vec_b) {return (*vec_a) + (*vec_b); },
			sol::meta_function::subtraction, [](const LuaInt64* vec_a, const LuaInt64* vec_b) {return (*vec_a) - (*vec_b); },
			sol::meta_function::multiplication, [](const LuaInt64* vec, const float a) {return (*vec) * a; },
			sol::meta_function::division, [](const LuaInt64* vec, const float a) {return (*vec) / a; },
			sol::meta_function::unary_minus, [](const LuaInt64* vec) {return (*vec) * -1; },
			sol::meta_function::equal_to, [](const LuaInt64* vec_a, const LuaInt64* vec_b) {return (*vec_a) == (*vec_b); }
		);
	}

	{
		ns_table.new_usertype<LuaUInt64>("UInt64", sol::call_constructor, sol::constructors<
			sol::types<>,
			sol::types<uint64>>(),
			sol::meta_function::to_string, [](const LuaUInt64* vec) -> std::string { return vec->toString(); },
			sol::meta_function::addition, [](const LuaUInt64* vec_a, const LuaUInt64* vec_b) {return (*vec_a) + (*vec_b); },
			sol::meta_function::subtraction, [](const LuaUInt64* vec_a, const LuaUInt64* vec_b) {return (*vec_a) - (*vec_b); },
			sol::meta_function::multiplication, [](const LuaUInt64* vec, const float a) {return (*vec) * a; },
			sol::meta_function::division, [](const LuaUInt64* vec, const float a) {return (*vec) / a; },
			sol::meta_function::unary_minus, [](const LuaUInt64* vec) {return (*vec) * -1; },
			sol::meta_function::equal_to, [](const LuaUInt64* vec_a, const LuaUInt64* vec_b) {return (*vec_a) == (*vec_b); }
		);
	}

	{	// IWindow
		ns_table.new_enum<IWindow::DialogType, true>("DialogType", {
				{"SingleFile",IWindow::DialogType::SingleFile},
				{"MultiFiles",IWindow::DialogType::MultiFiles},
				{"Folder",IWindow::DialogType::Folder},
				{"SaveFile",IWindow::DialogType::SaveFile}
			});
		ns_table.new_usertype<IWindow>("IWindow",
			"GetWindowSize", &IWindow::GetWindowSize,
			"SetWindowSize", &IWindow::SetWindowSize,
			"SetWindowPosition", &IWindow::SetWindowPosition,
			"DestroyWindow", &IWindow::DestroyWindow,
			"HideWindow", &IWindow::HideWindow,
			"MaximizeWindow", &IWindow::MaximizeWindow,
			"ShowWindow", [](IWindow& wnd) {
				return wnd.ShowWindow(true); 
			},
			"SetWindowTitle", &IWindow::SetWindowTitle,
			"GetContextVersion", &IWindow::GetContextVersion,
			"IsMainWindow", &IWindow::IsMainWindow,
			"IsFull", &IWindow::IsFull,
			"SetWindowMode", &IWindow::SetWindowMode,
			"Resizeview", &IWindow::Resizeview,
			"Resizeview", &IWindow::Resizeview,
			"GetFocus", &IWindow::GetFocus,
			"SetWindowShoudClose", &IWindow::SetWindowShoudClose,
			"GetWindowShoudClose", &IWindow::GetWindowShoudClose,
			"SetWindowCloseNow", &IWindow::SetWindowCloseNow,
			"GetWindowCloseNow", &IWindow::GetWindowCloseNow,
			"GetMonitorInfomation", [](IWindow& wnd, int idx) {
				int x, y, w, h;
				wnd.GetMonitorInfomation(0, &x, &y, &w, &h);
				return std::make_tuple(x, y, w, h);
			},
			"OpenFileDialog", [](IWindow& wnd, std::string& defaultPath) {
				std::replace(defaultPath.begin(), defaultPath.end(), '/', '\\');
				wnd.OpenFileDialog("", defaultPath, "", IWindow::SingleFile);
			},
			"OpenMultiFileDialog", [](IWindow& wnd, std::string& defaultPath) {
				std::replace(defaultPath.begin(), defaultPath.end(), '/', '\\');
				wnd.OpenFileDialog("", defaultPath, "", IWindow::MultiFiles);
			},
			"OpenFloderDialog", [](IWindow& wnd, std::string& defaultPath) {
				std::replace(defaultPath.begin(), defaultPath.end(), '/', '\\');
				wnd.OpenFileDialog("", defaultPath, "", IWindow::Folder);
			},
			"GetSelectedPath", [](IWindow& wnd) {
				std::vector<std::string>& paths = wnd.GetSelectedPath();
				for (std::string& path : paths)
				{
					replace(path.begin(), path.end(), '\\', '/');
				}
				return paths;
			}
			);
	}

	{	// IApplication
		ns_table.new_usertype<IApplication>("IApplication",
			"Instance", &IApplication::Instance,
			"GetMainWindow", &IApplication::GetMainWindow,
			"Abort", &IApplication::Abort
			);
	}

	{	// IFile
		ns_table.new_enum<IFile::ActionType, true>("ActionType", {
				{"AT_NONE",IFile::ActionType::AT_NONE},
				{"AT_READ",IFile::ActionType::AT_READ},
				{"AT_WRITE",IFile::ActionType::AT_WRITE}
			});

		ns_table.new_usertype<IFile>("IFile",
			"OpenFile", &IFile::OpenFile,
			"isOpen", &IFile::isOpen,
			"Seek", &IFile::Seek,
			"Tell", &IFile::Tell,
			"Flush", &IFile::Flush,
			"ReadFile", &IFile::ReadFile,
			"WriteFile", &IFile::WriteFile,
			"GetFilePath", &IFile::GetFilePath,
			"GetSize", &IFile::GetSize
		);
	}

	{	// IFileSystem
		ns_table.new_enum<IFileSystem::FileType, true>("FileType", {
				{"FT_NO_EXIST",IFileSystem::FileType::FT_NO_EXIST},
				{"FT_FILE",IFileSystem::FileType::FT_FILE},
				{"FT_DIR",IFileSystem::FileType::FT_DIR},
				{"FT_ZIP",IFileSystem::FileType::FT_ZIP}
			});

		ns_table.new_usertype<IFileSystem>("IFileSystem",
			"Instance", &IFileSystem::Instance,
			"FileFactory", &IFileSystem::FileFactory,
			"isFileExist", &IFileSystem::isFileExist,
			"PathAssembly", &IFileSystem::PathAssembly,
			"RecycleBin", &IFileSystem::RecycleBin,
			"GetTimestamp", &IFileSystem::GetTimestamp,
#ifdef _EDITOR
			"GetEdscPath", & IFileSystem::GetEdscPath,
			"GetAsstPath", & IFileSystem::GetAsstPath,
			"GetProjPath", & IFileSystem::GetProjPath,
			"SetAsstPath", & IFileSystem::SetAsstPath,
			"SetProjPath", & IFileSystem::SetProjPath,
#endif
			"GetRootPath", &IFileSystem::GetRootPath,
			"GetSavePath", &IFileSystem::GetSavePath,
			"GetScrsPath", &IFileSystem::GetScrsPath,
			"GetCommPath", &IFileSystem::GetCommPath,
			"GetDocmPath", &IFileSystem::GetDocmPath,
			"SetDocmPath", &IFileSystem::SetDocmPath
			);
	}

	{	// ILogSystem
		sol_state.set_function("LOGI", sol::overload([](const char* str) { ILogSystem::Instance()->ScriptLog(str); }));
		sol_state.set_function("LOGW", sol::overload([](const char* str) { ILogSystem::Instance()->ScriptWarning(str); }));
		sol_state.set_function("LOGE", sol::overload([](const char* str) { ILogSystem::Instance()->ScriptError(str); }));
		sol_state.set_function("CreateService", sol::overload([](const std::string& str) -> sol::table {
			auto vm = IScriptSystem::Instance()->VirtualMachineFactory();
			vm->DoLoadScript(str);
			sol::table* res = (sol::table*)vm->GetResult();
			return *res;
			}));
	}

	{	// ITimeSystem
		ns_table.new_usertype<ITimeSystem>("ITimeSystem",
			"Instance", &ITimeSystem::Instance,
			"GetGamePlayTime", &ITimeSystem::GetGamePlayTime,
			"UpdateFPS", &ITimeSystem::UpdateFPS,
			"GetFPS", &ITimeSystem::GetFPS,
			"GetDetTime", &ITimeSystem::GetDetTime,
			"GetTotalFrame", &ITimeSystem::GetTotalFrame
			);
	}

	{	// Rtti
		ns_table.new_usertype<Rtti>("Rtti",
			"GetName", &Rtti::GetName,
			"GetBase", &Rtti::GetBase,
			"IsSameType", &Rtti::IsSameType,
			"IsDerived", &Rtti::IsDerived
			); 
	}

	{	// Object
		ns_table.new_usertype<Object>("Object", sol::call_constructor, sol::constructors<sol::types<>, Object()>(),
			"GetType", &Object::GetType,
			"GetTypeName", &Object::GetTypeName,
			REGISTER_OBJECT_FUNC(Object),
			"IsSameType", sol::overload(
				(bool (Object::*)(const Object*) const)&Object::IsSameType,
				(bool (Object::*)(const Rtti&) const) & Object::IsSameType),
			"IsDerived", sol::overload(
				(bool (Object::*)(const Object*) const) & Object::IsDerived,
				(bool (Object::*)(const Rtti&) const) & Object::IsDerived),
			"GetObjectID", &Object::GetObjectID
			);
	}

	{	// Stream
#define DEFINE_ArchiveType(x)					{#x, Stream::x},
		ns_table.new_enum<Stream::ArchiveType, true>("ArchiveType", {
			DEFINE_ArchiveType(AT_SAVE)
			DEFINE_ArchiveType(AT_LOAD)
			DEFINE_ArchiveType(AT_LINK)
			DEFINE_ArchiveType(AT_REGISTER)
			DEFINE_ArchiveType(AT_SIZE)
			DEFINE_ArchiveType(AT_POSTLOAD)
			DEFINE_ArchiveType(AT_OBJECT_COLLECT_GC)
			DEFINE_ArchiveType(AT_CLEAR_OBJECT_PROPERTY_GC)
			DEFINE_ArchiveType(AT_LOAD_OBJECT_COLLECT_GC)
			});

		ns_table.new_usertype<Stream>("Stream", sol::call_constructor, sol::constructors<
			sol::types<>>(),
			"GetStreamFlag", &Stream::GetStreamFlag,
			"SetStreamFlag", &Stream::SetStreamFlag,
			"ArchiveAll", &Stream::ArchiveAll,
			"Load", &Stream::Load,
			"Save", &Stream::Save,
			"GetObjectByRtti", &Stream::GetObjectByRtti,
			"GetVersion", &Stream::GetVersion
			);
#undef DEFINE_ArchiveType
	}

	{
#define DEFINE_InputPropery(x)					{#x, InputPropery::x},
		ns_table.new_enum<int, true>("InputPropery", {
				DEFINE_InputPropery(MC_ARROW_CURSOR)
				DEFINE_InputPropery(MC_IBEAM_CURSOR)
				DEFINE_InputPropery(MC_CROSSHAIR_CURSOR)
				DEFINE_InputPropery(MC_HAND_CURSOR)
				DEFINE_InputPropery(MC_HRESIZE_CURSOR)
				DEFINE_InputPropery(MC_VRESIZE_CURSOR)
				DEFINE_InputPropery(MC_COUNT)

				DEFINE_InputPropery(KS_PRESS)
				DEFINE_InputPropery(KS_REPEAT)
				DEFINE_InputPropery(KS_RELEASE)

				DEFINE_InputPropery(KB_KEY_UNKOWN)
				DEFINE_InputPropery(KB_KEY_SPACE)
				DEFINE_InputPropery(KB_KEY_APOSTROPHE)
				DEFINE_InputPropery(KB_KEY_COMMA)
				DEFINE_InputPropery(KB_KEY_MINUS)
				DEFINE_InputPropery(KB_KEY_PERIOD)
				DEFINE_InputPropery(KB_KEY_SLASH)
				DEFINE_InputPropery(KB_KEY_0)
				DEFINE_InputPropery(KB_KEY_1)
				DEFINE_InputPropery(KB_KEY_2)
				DEFINE_InputPropery(KB_KEY_3)
				DEFINE_InputPropery(KB_KEY_4)
				DEFINE_InputPropery(KB_KEY_5)
				DEFINE_InputPropery(KB_KEY_6)
				DEFINE_InputPropery(KB_KEY_7)
				DEFINE_InputPropery(KB_KEY_8)
				DEFINE_InputPropery(KB_KEY_9)
				DEFINE_InputPropery(KB_KEY_SEMICOLON)
				DEFINE_InputPropery(KB_KEY_EQUAL)
				DEFINE_InputPropery(KB_KEY_A)
				DEFINE_InputPropery(KB_KEY_B)
				DEFINE_InputPropery(KB_KEY_C)
				DEFINE_InputPropery(KB_KEY_D)
				DEFINE_InputPropery(KB_KEY_E)
				DEFINE_InputPropery(KB_KEY_F)
				DEFINE_InputPropery(KB_KEY_G)
				DEFINE_InputPropery(KB_KEY_H)
				DEFINE_InputPropery(KB_KEY_I)
				DEFINE_InputPropery(KB_KEY_J)
				DEFINE_InputPropery(KB_KEY_K)
				DEFINE_InputPropery(KB_KEY_L)
				DEFINE_InputPropery(KB_KEY_M)
				DEFINE_InputPropery(KB_KEY_N)
				DEFINE_InputPropery(KB_KEY_O)
				DEFINE_InputPropery(KB_KEY_P)
				DEFINE_InputPropery(KB_KEY_Q)
				DEFINE_InputPropery(KB_KEY_R)
				DEFINE_InputPropery(KB_KEY_S)
				DEFINE_InputPropery(KB_KEY_T)
				DEFINE_InputPropery(KB_KEY_U)
				DEFINE_InputPropery(KB_KEY_V)
				DEFINE_InputPropery(KB_KEY_W)
				DEFINE_InputPropery(KB_KEY_X)
				DEFINE_InputPropery(KB_KEY_Y)
				DEFINE_InputPropery(KB_KEY_Z)
				DEFINE_InputPropery(KB_KEY_LEFT_BRACKET)
				DEFINE_InputPropery(KB_KEY_BACKSLASH)
				DEFINE_InputPropery(KB_KEY_RIGHT_BRACKET)
				DEFINE_InputPropery(KB_KEY_GRAVE_ACCENT)
				DEFINE_InputPropery(KB_KEY_WORLD_1)
				DEFINE_InputPropery(KB_KEY_WORLD_2)
				DEFINE_InputPropery(KB_KEY_ESCAPE)
				DEFINE_InputPropery(KB_KEY_ENTER)
				DEFINE_InputPropery(KB_KEY_TAB)
				DEFINE_InputPropery(KB_KEY_BACKSPACE)
				DEFINE_InputPropery(KB_KEY_INSERT)
				DEFINE_InputPropery(KB_KEY_RIGHT)
				DEFINE_InputPropery(KB_KEY_LEFT)
				DEFINE_InputPropery(KB_KEY_DOWN)
				DEFINE_InputPropery(KB_KEY_UP)
				DEFINE_InputPropery(KB_KEY_PAGE_UP)
				DEFINE_InputPropery(KB_KEY_PAGE_DOWN)
				DEFINE_InputPropery(KB_KEY_HOME)
				DEFINE_InputPropery(KB_KEY_END)
				DEFINE_InputPropery(KB_KEY_CAPS_LOCK)
				DEFINE_InputPropery(KB_KEY_SCROLL_LOCK)
				DEFINE_InputPropery(KB_KEY_NUM_LOCK)
				DEFINE_InputPropery(KB_KEY_PRINT_SCREEN)
				DEFINE_InputPropery(KB_KEY_PAUSE)
				DEFINE_InputPropery(KB_KEY_F1)
				DEFINE_InputPropery(KB_KEY_F2)
				DEFINE_InputPropery(KB_KEY_F3)
				DEFINE_InputPropery(KB_KEY_F4)
				DEFINE_InputPropery(KB_KEY_F5)
				DEFINE_InputPropery(KB_KEY_F6)
				DEFINE_InputPropery(KB_KEY_F7)
				DEFINE_InputPropery(KB_KEY_F8)
				DEFINE_InputPropery(KB_KEY_F9)
				DEFINE_InputPropery(KB_KEY_F10)
				DEFINE_InputPropery(KB_KEY_F11)
				DEFINE_InputPropery(KB_KEY_F12)
				DEFINE_InputPropery(KB_KEY_F13)
				DEFINE_InputPropery(KB_KEY_F14)
				DEFINE_InputPropery(KB_KEY_F15)
				DEFINE_InputPropery(KB_KEY_F16)
				DEFINE_InputPropery(KB_KEY_F17)
				DEFINE_InputPropery(KB_KEY_F18)
				DEFINE_InputPropery(KB_KEY_F19)
				DEFINE_InputPropery(KB_KEY_F20)
				DEFINE_InputPropery(KB_KEY_F21)
				DEFINE_InputPropery(KB_KEY_F22)
				DEFINE_InputPropery(KB_KEY_F23)
				DEFINE_InputPropery(KB_KEY_F24)
				DEFINE_InputPropery(KB_KEY_F25)
				DEFINE_InputPropery(KB_KEY_F25)
				DEFINE_InputPropery(KB_KEY_KP_0)
				DEFINE_InputPropery(KB_KEY_KP_1)
				DEFINE_InputPropery(KB_KEY_KP_2)
				DEFINE_InputPropery(KB_KEY_KP_3)
				DEFINE_InputPropery(KB_KEY_KP_4)
				DEFINE_InputPropery(KB_KEY_KP_5)
				DEFINE_InputPropery(KB_KEY_KP_6)
				DEFINE_InputPropery(KB_KEY_KP_7)
				DEFINE_InputPropery(KB_KEY_KP_8)
				DEFINE_InputPropery(KB_KEY_KP_9)
				DEFINE_InputPropery(KB_KEY_KP_DECIMAL)
				DEFINE_InputPropery(KB_KEY_KP_DIVIDE)
				DEFINE_InputPropery(KB_KEY_KP_MULTIPLY)
				DEFINE_InputPropery(KB_KEY_KP_SUBTRACT)
				DEFINE_InputPropery(KB_KEY_KP_ADD)
				DEFINE_InputPropery(KB_KEY_KP_ENTER)
				DEFINE_InputPropery(KB_KEY_KP_EQUAL)
				DEFINE_InputPropery(KB_KEY_LEFT_SHIFT)
				DEFINE_InputPropery(KB_KEY_LEFT_CONTROL)
				DEFINE_InputPropery(KB_KEY_LEFT_ALT)
				DEFINE_InputPropery(KB_KEY_LEFT_SUPER)
				DEFINE_InputPropery(KB_KEY_RIGHT_SHIFT)
				DEFINE_InputPropery(KB_KEY_RIGHT_CONTROL)
				DEFINE_InputPropery(KB_KEY_RIGHT_ALT)
				DEFINE_InputPropery(KB_KEY_RIGHT_SUPER)
				DEFINE_InputPropery(KB_KEY_MENU)
				DEFINE_InputPropery(KB_KEY_LAST)
			});
#undef DEFINE_InputPropery					
	}
}


NS_JYE_END

