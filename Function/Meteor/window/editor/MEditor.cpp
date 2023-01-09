
#include "MEditor.h"
#include "Core/Interface/IApplication.h"
#include "Core/Interface/IWindow.h"
#include "Core/Interface/ILogSystem.h"
#include "widget/mainmenu/MMainmenu.h"
#include "system/EditorSystem.h"
#include "system/WindowSystem.h"
#include "../project/MProjectManager.h"
#include "imgui.h"
#include "util/MImGuiUtil.h"

NS_JYE_BEGIN

const static int dw = 1280;
const static int dh = 720;

MEditor::MEditor()
	: m_mainmenu(nullptr)
	, m_loadResource(false)
{
	
}

MEditor::~MEditor()
{
	EditorSystem::Destroy();
	WindowSystem::Destroy();
}

void MEditor::ShowMainWindows()
{
	IWindow* mainWnd = IApplication::Instance()->GetMainWindow();
	mainWnd->ShowWindow(true);
	JYLOG("Windows system window size: %d   %d", dw, dh);
	mainWnd->SetWindowSize(dw, dh);
	int wx, wy, ww, wh;
	mainWnd->GetMonitorInfomation(0, &wx, &wy, &ww, &wh);
	int bx = (ww - dw) / 2;
	int by = (wh - dh) / 2;
	JYLOG("Windows system window pos: %d   %d", bx, by);
	mainWnd->SetWindowPosition(bx, by);
	mainWnd->MaximizeWindow();
}

void MEditor::Show()
{
	m_mainmenu = _NEW MMainmenu();
	EditorSystem::Create();
	WindowSystem::Create();

	EditorSystem::Instance()->OpenScene(MProjectManager::Instance()->GetSceneFullName(), RawDelegate<false, MEditor, void()>(this, &MEditor::ShowMainWindows));
}

MWindowIndex MEditor::Update(float dt)
{
	if (m_loadResource)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		if (viewport->Size.x < 500 || viewport->Size.y < 500)
		{
			ImGui::SetNextWindowSize(ImVec2(500, 500));
		} 
		else
		{
			ImGui::SetNextWindowSize(viewport->Size);
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		bool bopen = true;
		ImGui::Begin("Load Resource ...", &bopen, ImGuiWindowFlags_NoScrollbar
			+ ImGuiWindowFlags_NoBringToFrontOnFocus
			+ ImGuiWindowFlags_NoDocking
		);
		ImGui::OpenPopup("Load Resource(fbx,hdr)");
		if (ImGui::BeginPopupModal("Load Resource(fbx,hdr)", &bopen
			, ImGuiWindowFlags_NoMove
			+ ImGuiWindowFlags_NoScrollbar
		))
		{
			MImGuiUtil::CircleLoadingBar();
			ImGui::Separator();
			ImGui::Text("It may takes a long time...");
			ImGui::EndPopup();
		}

		ImGui::End();
		ImGui::PopStyleVar();

		return MWI_Editor;
	}

	if (_Docking(dt))
	{
		return MWI_Editor;
	}

	WindowSystem::Instance()->Update(dt);
	EditorSystem::Instance()->Update(dt);

	return MWI_Editor;
}

void MEditor::Close()
{

}

bool MEditor::_Docking(float dt)
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	int window_flags = ImGuiWindowFlags_MenuBar
		+ ImGuiWindowFlags_NoDocking
		+ ImGuiWindowFlags_NoTitleBar
		+ ImGuiWindowFlags_NoCollapse
		+ ImGuiWindowFlags_NoResize
		+ ImGuiWindowFlags_NoMove
		+ ImGuiWindowFlags_NoBringToFrontOnFocus
		+ ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	bool bopen = true;
	ImGui::Begin("Venus DockSpace", &bopen, window_flags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);
	ImGuiID dockspace_id = ImGui::GetID("Venus DockSpace");
	ImGui::DockSpace(dockspace_id);

	bool result = m_mainmenu->OnGui(dt);

	ImGui::End();

	return result;
}

NS_JYE_END