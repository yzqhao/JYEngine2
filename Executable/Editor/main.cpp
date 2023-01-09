

//#include "glad.h"
#include "Application.h"
#include "GlfwCallback.h"

#include <Windows.h>

USING_JYE_CC

GLFWwindow* g_Window;
AppWindow* g_Editor;
Application* g_pApplication;
std::thread::id main_thread_id;
static bool MAIN_THREAD_CVBO = true;
static bool MAIN_THREAD_CTEX = true;
static int VIEW_WIDTH = 8;
static int VIEW_HEIGHT = 8;
static int contextversion = 3;

int main()
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;
	
	// Create window with graphics context
	glfwWindowHint(GLFW_VISIBLE, false);	// Òþ²ØÖ÷´°¿Ú
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CLIENT_API, 0);
	g_Window = glfwCreateWindow(VIEW_WIDTH, VIEW_HEIGHT, "Engine", NULL, NULL);
	if (g_Window == NULL)
	{
		fprintf(stderr, "Failed to create window!\n");
		return 1;
	}
	//glfwMakeContextCurrent(g_Window);

	//setup window
	g_Editor = _NEW AppWindow(g_Window, false, VIEW_WIDTH, VIEW_HEIGHT);
	g_pApplication = _NEW Application(g_Editor);
	g_pApplication->SetConfigPath("root:editor_win.json");

	char strFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileNameA(NULL, strFilePath, MAX_PATH);
	(strrchr(strFilePath, ('\\')))[1] = 0;
	SetCurrentDirectoryA(strFilePath);
	String path(strFilePath);
	g_pApplication->OnInitial(path, path);

	Engine_KeyMapping();

	glfwSetKeyCallback(g_Window, Engine_KeyCallback);
	glfwSetCharCallback(g_Window, Engine_CharCallback);
	glfwSetMouseButtonCallback(g_Window, Engine_MouseButtonCallback);
	glfwSetCursorPosCallback(g_Window, Engine_MouseMoveCallback);
	glfwSetScrollCallback(g_Window, Engine_ScrollCallback);
	glfwSetWindowSizeCallback(g_Window, Engine_size_callback);\

	// Main loop
	while (!glfwWindowShouldClose(g_Window))
	{
		glfwPollEvents();

		g_Editor->FreshWindowFocus();
		if (!g_pApplication->Update())
		{
			break;
		}
		g_Editor->Update();
		
		if (glfwWindowShouldClose(g_Window))
		{
			glfwSetWindowShouldClose(g_Window, true);
		}
	}
	
	glfwDestroyWindow(g_Window);
	glfwTerminate();
	return 0;
}

