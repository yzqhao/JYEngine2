#include "AppWindow.h"
#include "GlfwCallback.h"

#include "nfd.h"
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#define GLFW_HAS_WINDOW_TOPMOST       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ GLFW_FLOATING
#define GLFW_HAS_WINDOW_HOVERED       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ GLFW_HOVERED
#define GLFW_HAS_WINDOW_ALPHA         (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwSetWindowOpacity
#define GLFW_HAS_PER_MONITOR_DPI      (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwGetMonitorContentScale
#define GLFW_HAS_VULKAN               (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwCreateWindowSurface
#define GLFW_HAS_FOCUS_WINDOW         (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwFocusWindow
#define GLFW_HAS_FOCUS_ON_SHOW        (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ GLFW_FOCUS_ON_SHOW
#define GLFW_HAS_MONITOR_WORK_AREA    (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwGetMonitorWorkarea

NS_JYE_BEGIN

std::map<uint64, IWindow*> AppWindow::s_WindowMap;

bool AppWindow::s_MainWindowCreated = false;

static void* glfwGetNativeWindowHandle(GLFWwindow* _window)
{
#if _PLATFORM_MAC
	return glfwGetCocoaWindow(_window);
#elif _PLATFORM_WINDOWS
	return glfwGetWin32Window(_window);
#endif
}

AppWindow::AppWindow(GLFWwindow* ud, bool owned, uint width, uint height)
	: IWindow()
	, m_isOwned(owned)
	, m_pWindow(ud)
{
	m_nativeWindow = glfwGetNativeWindowHandle(ud);

	s_WindowMap.insert({(int64)ud, this});

	m_w = width;
	m_h = height;

	if (false == s_MainWindowCreated)
	{
		s_MainWindowCreated = true;
		m_isMainWindow = true;
	}
}

AppWindow::~AppWindow()
{
	s_WindowMap.erase((intptr_t)m_pWindow);
}

void AppWindow::SetClipboardText(const char* text)
{
	glfwSetClipboardString(m_pWindow, text);
}

const char* AppWindow::GetClipboardText()
{
	return glfwGetClipboardString(m_pWindow);
}

void AppWindow::GetWindowSize(int* w, int* h)
{
	glfwGetWindowSize(m_pWindow, w, h);
}

void AppWindow::SetWindowSize(int x, int y)
{
	glfwSetWindowSize(m_pWindow, x, y);
}

void AppWindow::GetWindowPosition(int* x, int* y)
{
	glfwGetWindowPos(m_pWindow, x, y);
}

void AppWindow::SetWindowPosition(int x, int y)
{
	glfwSetWindowPos(m_pWindow, x, y);
}

void AppWindow::DestroyWindow()
{
	if (m_isOwned)
	{
		if (NULL != m_pWindow)
		{
			glfwDestroyWindow(m_pWindow);
			m_pWindow = nullptr;
			m_nativeWindow = nullptr;
		}
	}
}

void AppWindow::HideWindow()
{
	glfwHideWindow(m_pWindow);
}

void AppWindow::MaximizeWindow()
{
	glfwMaximizeWindow(m_pWindow);
}

void AppWindow::ShowWindow(bool taskbar)
{
#if defined(_WIN32)
	// GLFW hack: Hide icon from task bar
	HWND hwnd = glfwGetWin32Window(m_pWindow);
	if (!taskbar)
	{
		LONG ex_style = ::GetWindowLong(hwnd, GWL_EXSTYLE);
		ex_style &= ~WS_EX_APPWINDOW;
		ex_style |= WS_EX_TOOLWINDOW;
		::SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
	}

	// GLFW hack: install hook for WM_NCHITTEST message handler
#if GLFW_HAS_GLFW_HOVERED && defined(_WIN32)
	::SetPropA(hwnd, "IMGUI_VIEWPORT", viewport);
	if (g_GlfwWndProc == NULL)
		g_GlfwWndProc = (WNDPROC)::GetWindowLongPtr(hwnd, GWLP_WNDPROC);
	::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProcNoInputs);
#endif

#endif

	glfwShowWindow(m_pWindow);
}

void AppWindow::SetWindowTitle(const std::string& title)
{
	glfwSetWindowTitle(m_pWindow, title.c_str());
}

bool AppWindow::GetWindowMinimized()
{
	return glfwGetWindowAttrib(m_pWindow, GLFW_ICONIFIED) != 0;
}

void AppWindow::SwapBuffers()
{
	//glfwSwapBuffers(m_pWindow);
}

void AppWindow::SwapInterval(int i)
{
	//glfwSwapInterval(i);
}

int AppWindow::GetContextVersion()
{
	return 0;
}

uint AppWindow::GetMonitorCount()
{
	int monitors_count = 0;
	glfwGetMonitors(&monitors_count);
	return monitors_count;
}

void AppWindow::GetMonitorInfomation(uint index, int* x, int* y, int* w, int* h)
{
	int monitors_count = 0;
	GLFWmonitor** glfw_monitors = glfwGetMonitors(&monitors_count);
	glfwGetMonitorPos(glfw_monitors[index], x, y);
	const GLFWvidmode* vid_mode = glfwGetVideoMode(glfw_monitors[index]);
	*w = vid_mode->width;
	*h = vid_mode->height;
}

IWindow* AppWindow::CreateSubWindows(int x, int y, int w, int h, bool contextonly, bool nodecoration, bool topmost)
{
	IWindow* newwin = NULL;
	if (contextonly)
	{
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_CLIENT_API, 0);
		GLFWwindow* offscreen_context = glfwCreateWindow(16, 16, "", NULL, g_Window);
		if (offscreen_context)
		{
			newwin = _NEW AppWindow(offscreen_context, true, 16, 16);
		}
	}
	else
	{
		// GLFW 3.2 unfortunately always set focus on glfwCreateWindow() if GLFW_VISIBLE is set, regardless of GLFW_FOCUSED
		// With GLFW 3.3, the hint GLFW_FOCUS_ON_SHOW fixes this problem
		glfwWindowHint(GLFW_VISIBLE, false);
		glfwWindowHint(GLFW_FOCUSED, false);
		glfwWindowHint(GLFW_CLIENT_API, 0);
#if GLFW_HAS_FOCUS_ON_SHOW
		glfwWindowHint(GLFW_FOCUS_ON_SHOW, false);
#endif
		glfwWindowHint(GLFW_DECORATED, nodecoration);
#if GLFW_HAS_WINDOW_TOPMOST
		glfwWindowHint(GLFW_FLOATING, topmost);
#endif
		//GLFWwindow* share_window = glfwGetCurrentContext();
		GLFWwindow* share_window = m_pWindow;
		GLFWwindow* window = glfwCreateWindow(w, h, "engine editor", NULL, NULL);
		if (window)
		{
			newwin = _NEW AppWindow(window, true, w, h);
			glfwSetWindowPos(window, x, y);
			// Install callbacks for secondary viewports
			glfwSetKeyCallback(window, Engine_KeyCallback);
			glfwSetCharCallback(window, Engine_CharCallback);
			glfwSetMouseButtonCallback(window, Engine_MouseButtonCallback);
			glfwSetCursorPosCallback(window, Engine_MouseMoveCallback);
			glfwSetScrollCallback(window, Engine_ScrollCallback);
			glfwSetWindowSizeCallback(window, Engine_size_callback);
			glfwSetWindowCloseCallback(window, Venus_WindowCloseCallback);
			glfwSetWindowPosCallback(window, Venus_WindowPosCallback);

		}
	}
	return newwin;
}

void* AppWindow::GetWindowHanle()
{
	return m_pWindow;
}

void* AppWindow::GetNativeWindow()
{
	return m_nativeWindow;
}

bool AppWindow::IsMainWindow()
{
	return m_isMainWindow;
}

bool AppWindow::IsFull()
{
	return !m_bWindow;
}

void AppWindow::SetWindowMode(bool bWindow)
{
	if (m_bWindow != bWindow)
	{
		m_bWindow = bWindow;
	}
}

void AppWindow::SetWindowFocus()
{
	glfwFocusWindow(m_pWindow);
}

bool AppWindow::GetWindowFocus()
{
	return glfwGetWindowAttrib(m_pWindow, GLFW_FOCUSED) != 0;
}

static int _AttributeMapping(WindowPropery::Attribute wa)
{
	int glfwwa;
	switch (wa)
	{
	case WindowPropery::WA_FOCUSED: glfwwa = GLFW_FOCUSED; break;
	case WindowPropery::WA_RESIZABLE: glfwwa = GLFW_RESIZABLE; break;
	case WindowPropery::WA_VISIBLE: glfwwa = GLFW_VISIBLE; break;
	case WindowPropery::WA_DECORATED: glfwwa = GLFW_DECORATED; break;
	case WindowPropery::WA_AUTO_ICONIFY: glfwwa = GLFW_AUTO_ICONIFY; break;
	case WindowPropery::WA_FLOATING: glfwwa = GLFW_FLOATING; break;
	default:JYERROR("Attribute mapping failed! Unsupported window property!"); break;
	}
	return glfwwa;
}

void AppWindow::_SetWindowAttribute(WindowPropery::Attribute wa, int value)
{
	int glfwwa = _AttributeMapping(wa);
	glfwSetWindowAttrib(m_pWindow, glfwwa, value);
}

void AppWindow::SetWindowAttribute(WindowPropery::Attribute wa, int value)
{
	_SetWindowAttribute(wa, value);
}

int AppWindow::GetWindowAttribute(WindowPropery::Attribute wa)
{
	int glfwwa = _AttributeMapping(wa);
	return glfwGetWindowAttrib(m_pWindow, glfwwa);
}

void AppWindow::SetCursorPosition(double x, double y)
{
	glfwSetCursorPos(m_pWindow, x, y);
}

void AppWindow::GetCursorPosition(double* x, double* y)
{
	glfwGetCursorPos(m_pWindow, x, y);
}

int AppWindow::GetMouseButton(int index)
{
	return glfwGetMouseButton(m_pWindow, index);
}

bool AppWindow::_OpenFileDialog(const std::string& filterList, const std::string& defaultPath, const std::string& defaultName, DialogType dialogType)
{
	if (dialogType == IWindow::SingleFile)
	{
		nfdchar_t* out = nullptr;
		nfdresult_t result = NFD_OpenDialog(filterList.c_str(), defaultPath.c_str(), &out);
		if (result == NFD_OKAY)
		{
			m_selectedPath.push_back(out);
		}
		free(out);
	}
	else if (dialogType == IWindow::MultiFiles)
	{
		nfdpathset_t out;
		nfdresult_t result = NFD_OpenDialogMultiple(filterList.c_str(), defaultPath.c_str(), &out);
		if (result == NFD_OKAY)
		{
			auto selected_count = NFD_PathSet_GetCount(&out);
			for (size_t i = 0; i < selected_count; ++i)
			{
				nfdchar_t* path = NFD_PathSet_GetPath(&out, i);
				m_selectedPath.push_back(path);
			}
			NFD_PathSet_Free(&out);
		}
	}
	else if (dialogType == IWindow::Folder)
	{
		nfdchar_t* out = nullptr;
		nfdresult_t result = NFD_PickFolder(defaultPath.c_str(), &out);
		if (result == NFD_OKAY)
		{
			m_selectedPath.push_back(out);
			free(out);
		}
	}
	else if (dialogType == IWindow::SaveFile)
	{
		nfdchar_t* out;
		nfdresult_t result = NFD_SaveDialog(filterList.c_str(), defaultPath.c_str(), defaultName.c_str(), &out);
		if (result == NFD_OKAY)
		{
			m_selectedPath.push_back(out);
			free(out);
		}
	}
	return true;
}

void AppWindow::OpenFileDialog(const std::string& filterList, const std::string& defaultPath, const std::string& defaultName, DialogType dialogType)
{
	m_isOpenDialog = true;
	m_defaultPath = defaultPath;
	m_filter = filterList;
	m_dialogType = dialogType;
	m_defaultName = defaultName;
	m_selectedPath.clear();
}

std::vector<std::string>& AppWindow::GetSelectedPath()
{
	return m_selectedPath;
}

void AppWindow::FreshWindowFocus()
{
	m_isFocuse = GetWindowFocus();
}

bool AppWindow::GetFocus()
{
	return m_isFocuse;
}

void AppWindow::OnBeginTick()
{
	if (m_isOpenDialog)
	{
		if (_OpenFileDialog(m_filter, m_defaultPath, m_defaultName, m_dialogType))
		{
			m_isOpenDialog = false;
		}
	}
}

void AppWindow::OnEndTick()
{
}

IWindow* AppWindow::TryFindWindow(GLFWwindow* h)
{
	if (s_WindowMap.find((int64)h) != s_WindowMap.end())
	{
		return s_WindowMap.at((int64)h);
	}
	else
	{
		JYWARNING("TryFindWindow failed!");
	}
	return nullptr;
}

NS_JYE_END