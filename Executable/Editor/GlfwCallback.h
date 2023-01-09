#pragma once
#include "core/Propery/InputPropery.h"
#include "core/Propery/TouchInput.h"
#include "AppWindow.h"
#include "Application.h"
#include "Core/Interface/ILogSystem.h"

#include <GLFW/glfw3.h>


extern GLFWwindow* g_Window;

NS_JYE_BEGIN
	
static void glfw_error_callback(int error, const char* description)
{
	JYERROR("Glfw Error %d: %s\n", error, description);
}
	
static void Engine_KeyMapping()
{
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_SPACE, InputPropery::KB_KEY_SPACE);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_APOSTROPHE, InputPropery::KB_KEY_APOSTROPHE);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_COMMA, InputPropery::KB_KEY_COMMA);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_MINUS, InputPropery::KB_KEY_MINUS);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_PERIOD, InputPropery::KB_KEY_PERIOD);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_SLASH, InputPropery::KB_KEY_SLASH);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_0, InputPropery::KB_KEY_0);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_1, InputPropery::KB_KEY_1);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_2, InputPropery::KB_KEY_2);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_3, InputPropery::KB_KEY_3);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_4, InputPropery::KB_KEY_4);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_5, InputPropery::KB_KEY_5);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_6, InputPropery::KB_KEY_6);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_7, InputPropery::KB_KEY_7);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_8, InputPropery::KB_KEY_8);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_9, InputPropery::KB_KEY_9);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_SEMICOLON, InputPropery::KB_KEY_SEMICOLON);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_EQUAL, InputPropery::KB_KEY_EQUAL);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_A, InputPropery::KB_KEY_A);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_B, InputPropery::KB_KEY_B);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_C, InputPropery::KB_KEY_C);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_D, InputPropery::KB_KEY_D);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_E, InputPropery::KB_KEY_E);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F, InputPropery::KB_KEY_F);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_G, InputPropery::KB_KEY_G);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_H, InputPropery::KB_KEY_H);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_I, InputPropery::KB_KEY_I);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_J, InputPropery::KB_KEY_J);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_K, InputPropery::KB_KEY_K);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_L, InputPropery::KB_KEY_L);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_M, InputPropery::KB_KEY_M);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_N, InputPropery::KB_KEY_N);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_O, InputPropery::KB_KEY_O);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_P, InputPropery::KB_KEY_P);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_Q, InputPropery::KB_KEY_Q);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_R, InputPropery::KB_KEY_R);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_S, InputPropery::KB_KEY_S);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_T, InputPropery::KB_KEY_T);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_U, InputPropery::KB_KEY_U);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_V, InputPropery::KB_KEY_V);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_W, InputPropery::KB_KEY_W);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_X, InputPropery::KB_KEY_X);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_Y, InputPropery::KB_KEY_Y);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_Z, InputPropery::KB_KEY_Z);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_LEFT_BRACKET, InputPropery::KB_KEY_LEFT_BRACKET);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_BACKSLASH, InputPropery::KB_KEY_BACKSLASH);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_RIGHT_BRACKET, InputPropery::KB_KEY_RIGHT_BRACKET);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_GRAVE_ACCENT, InputPropery::KB_KEY_GRAVE_ACCENT);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_WORLD_1, InputPropery::KB_KEY_WORLD_1);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_WORLD_2, InputPropery::KB_KEY_WORLD_2);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_ESCAPE, InputPropery::KB_KEY_ESCAPE);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_ENTER, InputPropery::KB_KEY_ENTER);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_TAB, InputPropery::KB_KEY_TAB);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_BACKSPACE, InputPropery::KB_KEY_BACKSPACE);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_INSERT, InputPropery::KB_KEY_INSERT);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_DELETE, InputPropery::KB_KEY_DELETE);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_RIGHT, InputPropery::KB_KEY_RIGHT);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_LEFT, InputPropery::KB_KEY_LEFT);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_DOWN, InputPropery::KB_KEY_DOWN);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_UP, InputPropery::KB_KEY_UP);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_PAGE_UP, InputPropery::KB_KEY_PAGE_UP);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_PAGE_DOWN, InputPropery::KB_KEY_PAGE_DOWN);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_HOME, InputPropery::KB_KEY_HOME);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_END, InputPropery::KB_KEY_END);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_CAPS_LOCK, InputPropery::KB_KEY_CAPS_LOCK);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_SCROLL_LOCK, InputPropery::KB_KEY_SCROLL_LOCK);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_NUM_LOCK, InputPropery::KB_KEY_NUM_LOCK);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_PRINT_SCREEN, InputPropery::KB_KEY_PRINT_SCREEN);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_PAUSE, InputPropery::KB_KEY_PAUSE);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F1, InputPropery::KB_KEY_F1);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F2, InputPropery::KB_KEY_F2);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F3, InputPropery::KB_KEY_F3);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F4, InputPropery::KB_KEY_F4);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F5, InputPropery::KB_KEY_F5);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F6, InputPropery::KB_KEY_F6);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F7, InputPropery::KB_KEY_F7);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F8, InputPropery::KB_KEY_F8);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F9, InputPropery::KB_KEY_F9);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F10, InputPropery::KB_KEY_F10);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F11, InputPropery::KB_KEY_F11);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F12, InputPropery::KB_KEY_F12);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F13, InputPropery::KB_KEY_F13);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F14, InputPropery::KB_KEY_F14);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F15, InputPropery::KB_KEY_F15);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F16, InputPropery::KB_KEY_F16);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F17, InputPropery::KB_KEY_F17);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F18, InputPropery::KB_KEY_F18);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F19, InputPropery::KB_KEY_F19);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F20, InputPropery::KB_KEY_F20);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F21, InputPropery::KB_KEY_F21);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F22, InputPropery::KB_KEY_F22);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F23, InputPropery::KB_KEY_F23);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F24, InputPropery::KB_KEY_F24);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_F25, InputPropery::KB_KEY_F25);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_0, InputPropery::KB_KEY_KP_0);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_1, InputPropery::KB_KEY_KP_1);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_2, InputPropery::KB_KEY_KP_2);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_3, InputPropery::KB_KEY_KP_3);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_4, InputPropery::KB_KEY_KP_4);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_5, InputPropery::KB_KEY_KP_5);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_6, InputPropery::KB_KEY_KP_6);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_7, InputPropery::KB_KEY_KP_7);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_8, InputPropery::KB_KEY_KP_8);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_9, InputPropery::KB_KEY_KP_9);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_DECIMAL, InputPropery::KB_KEY_KP_DECIMAL);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_DIVIDE, InputPropery::KB_KEY_KP_DIVIDE);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_MULTIPLY, InputPropery::KB_KEY_KP_MULTIPLY);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_SUBTRACT, InputPropery::KB_KEY_KP_SUBTRACT);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_ADD, InputPropery::KB_KEY_KP_ADD);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_ENTER, InputPropery::KB_KEY_KP_ENTER);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_KP_EQUAL, InputPropery::KB_KEY_KP_EQUAL);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_LEFT_SHIFT, InputPropery::KB_KEY_LEFT_SHIFT);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_LEFT_CONTROL, InputPropery::KB_KEY_LEFT_CONTROL);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_LEFT_ALT, InputPropery::KB_KEY_LEFT_ALT);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_LEFT_SUPER, InputPropery::KB_KEY_LEFT_SUPER);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_RIGHT_SHIFT, InputPropery::KB_KEY_RIGHT_SHIFT);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_RIGHT_CONTROL, InputPropery::KB_KEY_RIGHT_CONTROL);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_RIGHT_ALT, InputPropery::KB_KEY_RIGHT_ALT);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_RIGHT_SUPER, InputPropery::KB_KEY_RIGHT_SUPER);
	Application::Instance()->RegisterKeyMapping(GLFW_KEY_MENU, InputPropery::KB_KEY_MENU);
}
	
static void Engine_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		Application::Instance()->OnKeyboard(key, InputPropery::KS_PRESS);
	}
	if (action == GLFW_RELEASE)
	{
		Application::Instance()->OnKeyboard(key, InputPropery::KS_RELEASE);
	}
}
	
static void Engine_CharCallback(GLFWwindow* window, unsigned int c)
{
	Application::Instance()->Char(c);
}
	
static void ToGameTouchPoint(GLFWwindow* window, double& outw, double& outh)
{
	int w, h;
	glfwGetWindowSize(window, &w, &h);
	outw = outw / w;
	outw = 2.0 * outw - 1.0;
	outh = outh / h;
	outh = 2.0 * outh - 1.0;
}
static double lpos_x(0.0);
static double lpos_y(0.0);
	
static void Engine_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	TouchInputs inputs;
	if (action == GLFW_PRESS)
	{
		inputs.push_back(TouchInput(0U, button, TouchInput::TT_PRESS, lpos_x, lpos_y));
		Application::Instance()->Touch(inputs);
	}
	else if (action == GLFW_RELEASE)
	{
		inputs.push_back(TouchInput(0U, button, TouchInput::TT_RELEASE, lpos_x, lpos_y));
		Application::Instance()->Touch(inputs);
	}
}
	
static void Engine_MouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
	TouchInputs inputs;
	ToGameTouchPoint(window, xpos, ypos);
	lpos_x = xpos;
	lpos_y = ypos;
	inputs.push_back(TouchInput(0U, nullhandle, TouchInput::TT_MOVE, xpos, ypos));
	Application::Instance()->Touch(inputs);
}
	
static void Engine_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Application::Instance()->Scroll(xoffset, yoffset);
}
	
static void Engine_size_callback(GLFWwindow* window, int width, int height)
{
	IWindow* w = AppWindow::TryFindWindow(window);
	w->Resizeview(0, 0, width, height, width, height);
}
	
static void Venus_WindowCloseCallback(GLFWwindow* window)
{
	IWindow* w = AppWindow::TryFindWindow(window);
	w->OnWindowCloseCallback(window);
}
	
static void Venus_WindowPosCallback(GLFWwindow* window, int, int)
{
	IWindow* w = AppWindow::TryFindWindow(window);
	w->OnWindowMoveCallback(window);
}

NS_JYE_END
