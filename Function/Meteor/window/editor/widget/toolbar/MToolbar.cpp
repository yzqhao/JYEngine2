#include "MToolbar.h"
#include "../../system/EditorSystem.h"
#include "../../system/MDefined.h"

#include "imgui.h"
#include "imguizmo.h"

NS_JYE_BEGIN

MToolbar::MToolbar()
	: MMemuWindowBase("Toolbar")
{

}

MToolbar::~MToolbar()
{

}

bool MToolbar::OnGui(float dt)
{
	if (IsOnGui())
	{
		int dockFlag = ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoCollapse;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		bool b = ImGui::Begin(m_titleName.c_str(), &m_isOnGui, dockFlag);

		DrawToolbar();

		ImGui::End();
		ImGui::PopStyleVar();
	}
	return true;
}

extern ImGui::operation g_operation;
void MToolbar::DrawToolbar()
{
	const Map<String, TextureEntity*>& Icons = EditorSystem::Instance()->GetIcons();
	bool showGrid = EditorSystem::Instance()->GetShowGrid();
	ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
	float contentRegionAvailWidth = contentRegionAvail.x;

	if (ImGui::ToolbarButtonEx((ImTextureID)Icons.find("Translate")->second, "TRANSLATE", g_operation == ImGui::translate ? true : false, true))
		g_operation = (ImGui::translate);
	ImGui::SameLine();
	if (ImGui::ToolbarButtonEx((ImTextureID)Icons.find("Rotation")->second, "ROTATE", g_operation == ImGui::rotate ? true : false, true))
		g_operation = (ImGui::rotate);
	ImGui::SameLine();
	if (ImGui::ToolbarButtonEx((ImTextureID)Icons.find("Scale")->second, "SCALE", g_operation == ImGui::scale ? true : false, true))
		g_operation = (ImGui::scale);
	ImGui::SameLine();

	if (ImGui::ToolbarButtonEx((ImTextureID)Icons.find("Grid")->second, "SHOW GRID", showGrid, true))
	{
		bool isShowGrid = (showGrid == false && true);
		EditorSystem::Instance()->SetShowGrid(isShowGrid);
	}
	ImGui::SameLine(contentRegionAvailWidth / 2.0 - 36.0, -1.0);
#if 0
	if (ImGui::ToolbarButtonEx((ImTextureID)Icons.find("Play")->second, "PLAY", EditorSystem::Instance()->IsPlay(), true))
	{
		if (EditorSystem::Instance()->IsPlay())
		{
			EditorSystem::Instance()->Stop();
		}
		else
		{
			//_SaveTempScene();
			EditorSystem::Instance()->Play();
		}
	}
	ImGui::SameLine();
	if (ImGui::ToolbarButtonEx((ImTextureID)Icons.find("Pause")->second, "PAUSE", EditorSystem::Instance()->IsPause(), true))
	{
		if (EditorSystem::Instance()->IsPause())
		{
			EditorSystem::Instance()->Play();
		}
		else
		{
			EditorSystem::Instance()->Pause();
		}
	}
	ImGui::SameLine();
	if (ImGui::ToolbarButtonEx((ImTextureID)Icons.find("Stop")->second, "STOP", false, true))
	{
		EditorSystem::Instance()->Stop();
	}
#endif
}

NS_JYE_END