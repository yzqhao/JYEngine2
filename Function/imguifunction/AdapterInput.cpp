#include "AdapterInput.h"
#include "Core/Interface/IWindow.h"
#include "Core/Interface/IApplication.h"
#include "Math/3DMath.h"

#include "imgui.h"

NS_JYE_BEGIN

static const char* ImGui_ImplGlfw_GetClipboardText(void* user_data)
{
	return static_cast<IWindow*>(user_data)->GetClipboardText();
}

static void ImGui_ImplGlfw_SetClipboardText(void* user_data, const char* text)
{
	static_cast<IWindow*>(user_data)->SetClipboardText(text);
}

SINGLETON_IMPLEMENT(AdapterInput);

AdapterInput::AdapterInput()
{
	for (int i = 0; i < AIC_MOUSE_COUNT; ++i)
	{
		m_MouseActionStatus[i] = false;
	}
}

AdapterInput::~AdapterInput()
{
}

void AdapterInput::_OnCreate()
{
	// Setup back-end capabilities flags
	ImGuiIO& io = ImGui::GetIO();

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = InputPropery::KB_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = InputPropery::KB_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = InputPropery::KB_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = InputPropery::KB_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = InputPropery::KB_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = InputPropery::KB_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = InputPropery::KB_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = InputPropery::KB_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = InputPropery::KB_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = InputPropery::KB_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = InputPropery::KB_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = InputPropery::KB_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = InputPropery::KB_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = InputPropery::KB_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = InputPropery::KB_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = InputPropery::KB_KEY_A;
	io.KeyMap[ImGuiKey_C] = InputPropery::KB_KEY_C;
	io.KeyMap[ImGuiKey_V] = InputPropery::KB_KEY_V;
	io.KeyMap[ImGuiKey_X] = InputPropery::KB_KEY_X;
	io.KeyMap[ImGuiKey_Y] = InputPropery::KB_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = InputPropery::KB_KEY_Z;

	io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
	io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
	io.ClipboardUserData = IApplication::Instance()->GetMainWindow();


	IApplication::Instance()->SetTouchCallback(DelegateEvent<void(const TouchInputs&)>::Handler::FromMethod<AdapterInput, &AdapterInput::OnTouch>(this));
	IApplication::Instance()->SetKeyboardCallback(DelegateEvent<void(InputPropery::Keyboard, InputPropery::KeyStatus)>::Handler::FromMethod<AdapterInput, &AdapterInput::OnKeyboard>(this));
	IApplication::Instance()->SetCharCallback(DelegateEvent<void(uint)>::Handler::FromMethod<AdapterInput, &AdapterInput::OnChar>(this));
	IApplication::Instance()->SetScrollCallback(DelegateEvent<void(float, float)>::Handler::FromMethod<AdapterInput, &AdapterInput::OnScroll>(this));
}

void AdapterInput::_OnDestroy()
{

}

void AdapterInput::OnChar(uint c)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(c);
}

void AdapterInput::OnKeyboard(InputPropery::Keyboard key, InputPropery::KeyStatus status)
{
	ImGuiIO& io = ImGui::GetIO();
	if (status == InputPropery::KS_PRESS)
		io.KeysDown[key] = true;
	if (status == InputPropery::KS_RELEASE)
		io.KeysDown[key] = false;

	// Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[InputPropery::KB_KEY_LEFT_CONTROL] || io.KeysDown[InputPropery::KB_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[InputPropery::KB_KEY_LEFT_SHIFT] || io.KeysDown[InputPropery::KB_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[InputPropery::KB_KEY_LEFT_ALT] || io.KeysDown[InputPropery::KB_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[InputPropery::KB_KEY_LEFT_SUPER] || io.KeysDown[InputPropery::KB_KEY_RIGHT_SUPER];
}

void AdapterInput::OnScroll(float x, float y)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += x;
	io.MouseWheel += y;
}

void AdapterInput::OnTouch(const TouchInputs& touchs)
{
	// Update buttons
	Math::Vec2 mousepos;
	TouchInput input = touchs[0];
	int button = input.button;

	if (TouchInput::TT_MOVE != input.type)
	{
		MouseActions& actionarray = m_MouseJustPressed[button];
		bool inputaction = TouchInput::TT_PRESS == input.type;
		if (actionarray.empty())
		{
			actionarray.push_back(inputaction);
		}
		else if (actionarray.back() != inputaction)
		{
			actionarray.push_back(inputaction);
		}
	}
}

void AdapterInput::EarlyUpdate()
{
	ImGuiIO& io = ImGui::GetIO();
	const ImVec2 mouse_pos_backup = io.MousePos;
	io.MouseHoveredViewport = 0;

	// Update buttons
	IWindow* mainwin = IApplication::Instance()->GetMainWindow();
	for (int i = 0; i < AIC_MOUSE_COUNT; ++i)
	{

		MouseActions& actionarray = m_MouseJustPressed[i];
		if (!actionarray.empty())
		{
			m_MouseActionStatus[i] = actionarray.front();
			actionarray.pop_front();
		}
		// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		io.MouseDown[i] = m_MouseActionStatus[i] || mainwin->GetMouseButton(i) != 0;
		//m_MouseJustPressed[i] = false;
	}

	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	for (int n = 0; n < platform_io.Viewports.Size; n++)
	{
		ImGuiViewport* viewport = platform_io.Viewports[n];
		IWindow* window = static_cast<IWindow*>(viewport->PlatformUserData);
		IM_ASSERT(window != NULL);
		const bool focused = window->GetWindowAttribute(WindowPropery::WA_FOCUSED) != 0;
		if (focused)
		{
			if (io.WantSetMousePos)
			{
				window->SetCursorPosition(
					(double)(mouse_pos_backup.x - viewport->Pos.x),
					(double)(mouse_pos_backup.y - viewport->Pos.y));
			}
			else
			{
				double mouse_x, mouse_y;
				window->GetCursorPosition(&mouse_x, &mouse_y);
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					// Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
					int window_x, window_y;
					window->GetWindowPosition(&window_x, &window_y);
					io.MousePos = ImVec2((float)mouse_x + window_x, (float)mouse_y + window_y);
				}
				else
				{
					// Single viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
					io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
				}
			}
		}
	}
}

void AdapterInput::LateUpdate()
{
}

NS_JYE_END