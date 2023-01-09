#include "MHierarchyBaseView.h"
#include "imgui.h"
#include "Engine/object/GObject.h"
#include "Engine/object/Scene.h"
#include "Engine/component/camera/CameraComponent.h"
#include "util/MSceneManagerExt.h"
#include "../../system/EditorSystem.h"
#include "../scene/MDrag.h"

NS_JYE_BEGIN

MHierarchyBaseView::MHierarchyBaseView()
	: m_bShowDeletePromptDialog(false)
	, m_currentID(false)
	, m_scene(nullptr)
	, m_deleteNode(nullptr)
{

}

MHierarchyBaseView::~MHierarchyBaseView()
{

}

void MHierarchyBaseView::DeletePrompt(GObject* node, DeleteNodeDelegate& deletenodecall)
{
	m_bShowDeletePromptDialog = true;
	m_deleteNodeCall = std::move(deletenodecall);
	m_deleteNode = node;
}

void MHierarchyBaseView::ShowDeletePrompt()
{
	if (!m_bShowDeletePromptDialog)
	{
		return;
	}

	if (m_deleteNode)
	{
		CameraComponent* camComp = m_deleteNode->TryGetComponent<CameraComponent>();
		CameraComponent* editCamComp = MSceneManagerExt::Instance()->GetEditScene()->GetMainCamera();
		if (camComp && editCamComp)
		{
			// 删除主相机的node提示
			if (camComp == editCamComp)
			{
				uint windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking;
				ImGui::SetNextWindowSize(ImVec2(0, 0));
				bool show = true;
				bool b1 = ImGui::Begin("Warning", &show, windowFlags);
				ImGui::Text("Main camera will be deletetd?\n If select ok,you shoule select a new camera for scene!");
				ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
				float contentRegionAvailWidth = contentRegionAvail.x;
				float contentRegionAvailHeight = contentRegionAvail.y;
				ImVec2 currentCursorPos = ImGui::GetCursorPos();
				ImGui::SetCursorPos(ImVec2(currentCursorPos.x + contentRegionAvailWidth / 2.5, currentCursorPos.y + contentRegionAvailHeight / 2));
				if (ImGui::Button("Ok "))
				{
					m_deleteNodeCall.Execute(m_deleteNode);
					m_bShowDeletePromptDialog = false;
					m_scene->SetMainCamera(nullptr);
					Scene* gameScene = nullptr;// EditorSystem:GetGameScene();
					if (gameScene)
					{
						gameScene->SetMainCamera(nullptr);
					}
				}
			}
		}
		else
		{
			m_deleteNodeCall.Execute(m_deleteNode);
			m_bShowDeletePromptDialog = false;
		}
	}
}

MHierarchyBaseView::TMHierarchyAction MHierarchyBaseView::_CheckContexMenu(GObject* node)
{
	TMHierarchyAction action = TMHierarchyAction_None;
	if (node)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		if (ImGui::BeginPopupContextItem("Entity Context Menu", 1))
		{
			EditorSystem::Instance()->Select(node);

			if (IsAllowRename(node) && ImGui::MenuItem("Rename", "F2", false, true))  //重命名
				action = TMHierarchyAction_Rename;

			if (node->GetStaticID() != m_scene->GetRootNode()->GetStaticID())
			{
				if (ImGui::MenuItem("Delete", "DEL", false, IsAllowDelete(node)))
				{
					DeletePrompt(node, DeleteNodeDelegate::CreateLambda([](GObject* deletenode) {
						if (MDrag::GetTheObjectID() == deletenode->GetStaticID())  //要删除的节点和当前选中的节点是同一个节点
						{
							MDrag::DropObject();
							EditorSystem::Instance()->Select((GObject*)nullptr);
						}
						EditorSystem::Instance()->DestroyNodeTree(deletenode);
					}));
				}
			}

			if (ImGui::MenuItem("Clone", "F3", false, true))  //重命名
				action = TMHierarchyAction_Clone;

			ImGui::EndPopup();
		}
		ImGui::PopStyleVar();
	}
	return action;
}

bool MHierarchyBaseView::IsAllowRename(GObject* node)
{
	return true;
}

bool MHierarchyBaseView::IsAllowDelete(GObject* node)
{
	return true;
}

NS_JYE_END