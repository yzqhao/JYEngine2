#include "MHierarchy.h"
#include "MHierarchyResManager.h"

#include "imgui.h"
#include "MHierarchyTool.h"
#include "MHierarchyScene.h"

NS_JYE_BEGIN

MHierarchy::MHierarchy()
	: MMemuWindowBase("hierarchy")
	, m_bshowPrefabDialog(false)
	, m_bshownContextMenu(false)
	, m_curViewType(MHierarchyBaseView::TMHierarchyView_Scene)
{
	MHierarchyResManager::Instance()->LoadResources();

	m_views.resize(MHierarchyBaseView::TMHierarchyView_Count);
	m_views[MHierarchyBaseView::TMHierarchyView_Scene] = _NEW MHierarchyScene();
}

MHierarchy::~MHierarchy()
{
}

void MHierarchy::ClearScene()
{
	for (int i = 0; i < m_views.size(); ++i)
	{
		if (m_views[i])
		{
			m_views[i]->ClearScene();
		}
	}
}

bool MHierarchy::OnGui(float dt)
{
	if (IsOnGui())
	{
		int dockFlag = ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoCollapse;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 8));
		bool b = ImGui::Begin(m_titleName.c_str(), &m_isOnGui, dockFlag);

		ImGui::BeginChild("background"); // 处理拖拽到空白处的逻辑
		ImGui::InvisibleButton("#invisivle", ImVec2(0.1, 4));
		ImGui::SameLine();

		auto& resInfo = MHierarchyResManager::Instance()->GetIconRes(M_Hierarchy_New);
		bool clicked = ImGui::ImageButtonEx(resInfo.m_tex, ImVec2(20, 20), resInfo.m_tips.c_str(), false, true);
		if (clicked || m_bshownContextMenu)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
			m_bshownContextMenu = MHierarchyTool::NewNodeContextMenu();
			ImGui::PopStyleVar();
		}

		if (m_curViewType == MHierarchyBaseView::TMHierarchyView_Scene)
			m_views[m_curViewType]->OnGui(dt);

		ImGui::EndChild();
		ImGui::End();
		ImGui::PopStyleVar();
	}
	return true;
}

NS_JYE_END