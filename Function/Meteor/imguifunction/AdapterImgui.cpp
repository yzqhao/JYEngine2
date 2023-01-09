#include "AdapterImgui.h"
#include "Engine/object/SceneManager.h"
#include "Core/Configure.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/ITimeSystem.h"
#include "Core/Interface/IApplication.h"
#include "Core/Interface/IWindow.h"
#include "Engine/Engine.h"
#include "AdapterWindow.h"
#include "AdapterInput.h"

#include "imgui.h"
#include "misc/freetype/imgui_freetype.h"
#include "misc/freetype/imgui_freetype.cpp"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(AdapterImgui);

AdapterImgui::AdapterImgui()
	:m_CumulateTime(0.0)
	, m_pAdapterWindow(NULL)
{

}

AdapterImgui::~AdapterImgui()
{

}

void AdapterImgui::_OnCreate()
{
	Engine::Instance()->AddEarlyUpdateCallback(MulticastDelegate<>::DelegateT::CreateRaw(this, &AdapterImgui::EarlyUpdate));
	Engine::Instance()->AddLateUpdateCallback(MulticastDelegate<>::DelegateT::CreateRaw(this, &AdapterImgui::LateUpdate));

	SceneManager::Instance()->CreateScene("EditorUI");
	// Setup Dear ImGui context
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
#ifdef _PLATFORM_WINDOWS
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
#endif
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;    // We can create multi-viewports on the Renderer side (optional)
	io.BackendPlatformName = "JY";
	io.BackendRendererName = "render";

	//change imgui font(chinese supply)
	std::string realPath = IFileSystem::Instance()->PathAssembly(Configure::ARIALUNI_FONT);
	io.Fonts->AddFontFromFileTTF(realPath.c_str(), 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());

	unsigned int FontsFlags = ImGuiFreeType::MonoHinting | ImGuiFreeType::Monochrome;

	for (auto n = 0; n < io.Fonts->ConfigData.Size; n++)
	{
		auto* font_config = static_cast<ImFontConfig*>(&io.Fonts->ConfigData[n]);
		font_config->RasterizerMultiply = 1.5f;
		font_config->RasterizerFlags = FontsFlags;
	}
	ImGuiFreeType::BuildFontAtlas(io.Fonts, FontsFlags);

	// Register main window handle (which is owned by the main application, not by us)
	IWindow* mainwindow = IApplication::Instance()->GetMainWindow();
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	m_pAdapterWindow = _NEW AdapterWindow(mainwindow, main_viewport);

	ImGuiStyle& style = ImGui::GetStyle();

	// RGB(9,9,9)
	style.Colors[ImGuiCol_ChildBg] = ImVec4(9.0f / 0xff, 9.0f / 0xff, 9.0f / 0xff, 1);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(9.0f / 0xff, 9.0f / 0xff, 9.0f / 0xff, 1);

	// RGB(51,51,52)
	style.Colors[ImGuiCol_TabHovered] = ImVec4(96.0f / 0xff, 96.0f / 0xff, 96.0f / 0xff, 1);
	style.Colors[ImGuiCol_Tab] = ImVec4(51.0f / 0xff, 51.0f / 0xff, 51.0f / 0xff, 1);
	style.Colors[ImGuiCol_TabActive] = ImVec4(128.0f / 0xff, 128.0f / 0xff, 128.0f / 0xff, 1);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(51.0f / 0xff, 51.0f / 0xff, 51.0f / 0xff, 1);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(51.0f / 0xff, 51.0f / 0xff, 51.0f / 0xff, 1);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(51.0f / 0xff, 51.0f / 0xff, 51.0f / 0xff, 1);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(51.0f / 0xff, 51.0f / 0xff, 51.0f / 0xff, 1);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(51.0f / 0xff, 51.0f / 0xff, 51.0f / 0xff, 1);

	style.PopupBorderSize = 0;
	style.ChildBorderSize = 0;
	style.TabRounding = 0;
	style.WindowRounding = 0;

	style.ItemInnerSpacing = ImVec2(0, 4);

	AdapterWindow::SetupWindow();
	AdapterInput::Create();
	AdapterRender::SetupRender();

	m_CumulateTime = ITimeSystem::Instance()->GetGamePlayTime();
}

void AdapterImgui::_OnDestroy()
{
	// Cleanup
	AdapterInput::Destroy();
	ImGui::DestroyContext();
}

double AdapterImgui::_GetTimespan()
{
	double now = ITimeSystem::Instance()->GetGamePlayTime();
	double diff = now - m_CumulateTime;
	m_CumulateTime = now;
	return diff;
}

void AdapterImgui::EarlyUpdate()
{
	ImGuiIO& io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");
	Math::IntVec2 resolution = Engine::Instance()->GetMainResolution();
	Math::IntVec4 viewSize = Engine::Instance()->GetMainViewSize();
	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	io.DisplaySize = ImVec2(viewSize.z, viewSize.w);
	if (resolution.x == viewSize.z)
	{
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	}
	else
	{
		io.DisplayFramebufferScale = ImVec2(2.0f, 2.0f);
	}
	io.DeltaTime = _GetTimespan();
	AdapterInput::Instance()->EarlyUpdate();
	ImGui::NewFrame();
}

void AdapterImgui::LateUpdate()
{
	if (!ImGui::GetCurrentWindowRead())
		return;

	ImGui::Render();

	m_pAdapterWindow->Render(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	AdapterInput::Instance()->LateUpdate();
}

NS_JYE_END