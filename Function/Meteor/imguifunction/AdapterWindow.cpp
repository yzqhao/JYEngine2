#include "AdapterWindow.h"
#include "Core/Interface/IWindow.h"
#include "Engine/IGraphicSystem.h"

NS_JYE_BEGIN

static void ImGui_ImplGlfw_CreateWindow(ImGuiViewport* viewport)
{
	IWindow* mainwindow = IApplication::Instance()->GetMainWindow();
	IWindow* newwindow =
		mainwindow->CreateSubWindows(
			(int)viewport->Pos.x, (int)viewport->Pos.y,
			(int)viewport->Size.x, (int)viewport->Size.y,
			false,
			(viewport->Flags & ImGuiViewportFlags_NoDecoration) ? false : true,
			(viewport->Flags & ImGuiViewportFlags_TopMost) ? true : false);
	IGraphicSystem::Instance()->InsertRenderWindow(newwindow);
	AdapterWindow* awin = new AdapterWindow(newwindow, viewport);
	viewport->PlatformUserData = newwindow;
	viewport->PlatformHandle = awin;
}

static void ImGui_ImplGlfw_DestroyWindow(ImGuiViewport* viewport)
{
	IWindow* window = static_cast<IWindow*>(viewport->PlatformUserData);
	IGraphicSystem::Instance()->RemoveRenderWindow(window);
	window->DestroyWindow();
	AdapterWindow* awin = static_cast<AdapterWindow*>(viewport->PlatformHandle);
	awin->OnWindowDestroy();
	SAFE_DELETE(window);
	SAFE_DELETE(awin);
	viewport->PlatformUserData = NULL;
	viewport->PlatformHandle = NULL;
}

static void ImGui_ImplGlfw_ShowWindow(ImGuiViewport* viewport)
{
	static_cast<IWindow*>(viewport->PlatformUserData)
		->ShowWindow(0 == (viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon));
}

static ImVec2 ImGui_ImplGlfw_GetWindowPos(ImGuiViewport* viewport)
{
	int x, y;
	static_cast<IWindow*>(viewport->PlatformUserData)->GetWindowPosition(&x, &y);
	return ImVec2((float)x, (float)y);
}

static void ImGui_ImplGlfw_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
	static_cast<IWindow*>(viewport->PlatformUserData)->SetWindowPosition(pos.x, pos.y);
}

static ImVec2 ImGui_ImplGlfw_GetWindowSize(ImGuiViewport* viewport)
{
	int x, y;
	static_cast<IWindow*>(viewport->PlatformUserData)->GetWindowSize(&x, &y);
	return ImVec2((float)x, (float)y);
}

static void ImGui_ImplGlfw_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
	static_cast<IWindow*>(viewport->PlatformUserData)->SetWindowSize(size.x, size.y);
}

static void ImGui_ImplGlfw_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
	static_cast<IWindow*>(viewport->PlatformUserData)->SetWindowTitle(title);
}

static void ImGui_ImplGlfw_SetWindowFocus(ImGuiViewport* viewport)
{
	static_cast<IWindow*>(viewport->PlatformUserData)->SetWindowFocus();
}

static bool ImGui_ImplGlfw_GetWindowFocus(ImGuiViewport* viewport)
{
	return static_cast<IWindow*>(viewport->PlatformUserData)->GetWindowFocus();
}

static bool ImGui_ImplGlfw_GetWindowMinimized(ImGuiViewport* viewport)
{
	return static_cast<IWindow*>(viewport->PlatformUserData)->GetWindowMinimized();
}

static void ImGui_ImplGlfw_MakeCurrent(ImGuiViewport* viewport, void* ud)
{

}

static void ImGui_ImplGlfw_ResetCache(ImGuiViewport* viewport, void* ud)
{

}

static void ImGui_ImplGlfw_SwapBuffers(ImGuiViewport* viewport, void* ud)
{

}

static void ImGui_RenderDrawData(ImGuiViewport* viewport, void*)
{
	AdapterWindow* awin = static_cast<AdapterWindow*>(viewport->PlatformHandle);
	awin->Render(viewport->DrawData);
}


AdapterWindow::AdapterWindow(IWindow* w, ImGuiViewport* vp)
	:m_rpHostWindow(w)
	, m_pAdapterRender(_NEW AdapterRender(w))
	, m_rpImGuiViewport(vp)
{
	vp->PlatformUserData = w;
	vp->PlatformHandle = this;

	m_rpHostWindow->SetWindowCloseCallback(MulticastDelegate<void*>::DelegateT::CreateRaw(this, &AdapterWindow::OnWindowClose));
	m_rpHostWindow->SetWindowMoveCallback(MulticastDelegate<void*>::DelegateT::CreateRaw(this, &AdapterWindow::OnWindowMove));
}

AdapterWindow::~AdapterWindow()
{
	SAFE_DELETE(m_pAdapterRender);
}

void AdapterWindow::OnWindowClose(void* window)
{
	m_rpImGuiViewport->PlatformRequestClose = true;
}

void AdapterWindow::OnWindowMove(void* window)
{
	m_rpImGuiViewport->PlatformRequestMove = true;
}

void AdapterWindow::OnWindowDestroy()
{
	m_pAdapterRender->OnWindowDestroy();
}

void AdapterWindow::SetupWindow()
{
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.Renderer_RenderWindow = ImGui_RenderDrawData;

	// Register platform interface (will be coupled with a renderer interface)
	platform_io.Platform_CreateWindow = ImGui_ImplGlfw_CreateWindow;
	platform_io.Platform_DestroyWindow = ImGui_ImplGlfw_DestroyWindow;
	platform_io.Platform_ShowWindow = ImGui_ImplGlfw_ShowWindow;
	platform_io.Platform_SetWindowPos = ImGui_ImplGlfw_SetWindowPos;
	platform_io.Platform_GetWindowPos = ImGui_ImplGlfw_GetWindowPos;
	platform_io.Platform_SetWindowSize = ImGui_ImplGlfw_SetWindowSize;
	platform_io.Platform_GetWindowSize = ImGui_ImplGlfw_GetWindowSize;
	platform_io.Platform_SetWindowFocus = ImGui_ImplGlfw_SetWindowFocus;
	platform_io.Platform_GetWindowFocus = ImGui_ImplGlfw_GetWindowFocus;
	platform_io.Platform_GetWindowMinimized = ImGui_ImplGlfw_GetWindowMinimized;
	platform_io.Platform_SetWindowTitle = ImGui_ImplGlfw_SetWindowTitle;
	platform_io.Platform_RenderWindow = ImGui_ImplGlfw_ResetCache;
	//platform_io.Platform_SwapBuffers = ImGui_ImplGlfw_SwapBuffers;//swap在pipeline的时候进行，这里不需要

	// FIXME-PLATFORM: GLFW doesn't export monitor work area (see https://github.com/glfw/glfw/pull/989)
	IWindow* mainwindow = IApplication::Instance()->GetMainWindow();
	int monitors_count = mainwindow->GetMonitorCount();
	platform_io.Monitors.resize(0);
	for (int n = 0; n < monitors_count; n++)
	{
		int x, y, w, h;
		ImGuiPlatformMonitor monitor;
		mainwindow->GetMonitorInfomation(n, &x, &y, &w, &h);
		monitor.MainPos = monitor.WorkPos = ImVec2((float)x, (float)y);
		monitor.MainSize = monitor.WorkSize = ImVec2((float)w, (float)h);
		platform_io.Monitors.push_back(monitor);
	}
}

NS_JYE_END