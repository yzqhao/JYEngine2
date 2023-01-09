#include "MInspector.h"
#include "../../system/EditorSystem.h"
#include "panels/MPTransformComponent.h"
#include "panels/MPRenderComponent.h"
#include "panels/MPLightComponent.h"
#include "panels/MPCameraComponent.h"
#include "Engine/object/Scene.h"
#include "Core/RttiManager.h"
#include "Engine/component/render/RenderComponent.h"
#include "../../command/MCommandManager.h"
#include "../../command/commands/MCmdPropertyChange.h"
#include "../../command/commands/node/MCmdCreateComponent.h"
#include "../../command/commands/node/MCmdCreateRenderComponent.h"
#include "../../system/MDefined.h"
#include "panels/assets/MAssetsGui.h"

#include "imgui.h"

NS_JYE_BEGIN

MInspector::MInspector()
	: MMemuWindowBase("Inspector")
{
#define REGISTER_MP_PANEL(type) \
	{\
		type* panel = _NEW type();	\
		m_panels.insert({ panel->GetPanelName(), panel });	\
	}

	REGISTER_MP_PANEL(MPTransformComponent);
	REGISTER_MP_PANEL(MPRenderComponent);
	REGISTER_MP_PANEL(MPLightComponent);
	REGISTER_MP_PANEL(MPCameraComponent);
}

MInspector::~MInspector()
{

}

bool MInspector::OnGui(float dt)
{
	if (IsOnGui())
	{
		int dockFlag = ImGuiWindowFlags_NoScrollbar + ImGuiWindowFlags_NoCollapse;
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 5);
		bool b = ImGui::Begin(m_titleName.c_str(), &m_isOnGui, dockFlag);
		
		GObject* selectGO = EditorSystem::Instance()->GetSelectedGObject();
		Scene* selectSce = EditorSystem::Instance()->GetSelectedScene();
		String* selectStr = EditorSystem::Instance()->GetSelectedString();
		if (selectGO)
		{
			_OnNode(selectGO);
		}
		else if (selectSce)
		{
			_OnScene(selectSce);
		}
		else if (selectStr)
		{
			MAssetsGui::OnGui(*selectStr);
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}
	return true;
}

void MInspector::_OnNode(GObject* node)
{
	uint64 hash = node->GetObjectID();
	String label = "Node##" + std::to_string(hash);

	if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		uint64 StaticID = node->GetStaticID();
		bool isactive = node->isActive();
		bool activechange = ImGui::Checkbox("Active", &isactive);
		if (activechange)
		{
			if (activechange)
			{
				M_CMD_EXEC_BEGIN;
				MCommandManager::Instance()->Execute<MCmdPropertyChangebool>(StaticID, "m_isActive", isactive);
				M_CMD_EXEC_END;
			}
		}
		if (EditorSystem::Instance()->ShowEverything())
		{
			ImGui::Text("EditorUIType");
			ImGui::SameLine();
			Animatable::EditorUIType uiType = node->GetEditorUIType();
			String defaultName = MDefined::mapEditorUIType.find(uiType)->second;
			if (ImGui::BeginCombo(" ", defaultName.c_str()))
			{
				for (auto& it : MDefined::mapEditorUIType)
				{
					bool isSelected = uiType == it.first;
					if (ImGui::Selectable(it.second.c_str(), isSelected))
					{
						M_CMD_EXEC_BEGIN;
						MCommandManager::Instance()->Execute<MCmdPropertyChangeint>(StaticID, "m_EditorUIType", it.first);
						M_CMD_EXEC_END;
					}
				}
				ImGui::EndCombo();
			}
		}
		ImGui::TreePush(label.c_str());
		if (ImGui::TreeNode("Layer"))
		{
			// TODO layer
			ImGui::TreePop();
		}
		ImGui::Text("Static ID: %llu", StaticID);
		ImGui::Text("PrefabPath: %s", node->GetPrefabPath().c_str());
		ImGui::Text("HostPrefabPath: %s", node->GetHostPrefabPath().c_str());

		_OnComponents(node);

		ImGui::Text("Name");
		ImGui::SameLine();

		static char bufnodename[64] = {};
		strcpy(bufnodename, node->GetName().c_str());
		bool bR = ImGui::InputText(("##" + std::to_string(hash)).c_str(), bufnodename, 64, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue);
		if (bR)
		{
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdPropertyChangeString>(StaticID, "m_name", String(bufnodename));
			M_CMD_EXEC_END;
		}
		_AddComponent(node);
		ImGui::TreePop();
	}
}

void MInspector::_OnScene(Scene* sce)
{

}

void MInspector::_OnComponents(GObject* node)
{
	auto& coms = node->GetComponents();
	for (auto& it : coms)
	{
		MPanelBase* panel = m_panels[it->GetTypeName()];
		panel->OnGui(it, 0.0);
	}
}

void MInspector::_AddComponent(GObject* node)
{
	uint64 hash = node->GetObjectID();
	_AddComponentPopup(node);
	ImGui::Separator();
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 3);
	float ButtonWidth = ImGui::GetWindowWidth() / 3;
	if (ImGui::Button("Add Component", ImVec2(ButtonWidth, 0.0)))
		ImGui::OpenPopup("Add Component");
	
}

void MInspector::_AddComponentPopup(GObject* node)
{
	uint64 hash = node->GetObjectID();
	const String& icom = Component::RTTI().GetName();
	Vector<String> allcomname;
	RttiManager::Instance()->GetChildrenName(icom, allcomname);
	
	auto& coms = node->GetComponents();
	for (auto& itNameCom : coms)
	{
		for (auto it = allcomname.begin(); it != allcomname.end();) 
		{
			if (*it == itNameCom->GetTypeName()) 
				it = allcomname.erase(it);
			else 
				it++;
		}
	}

	if (ImGui::BeginPopup("Add Component"))
	{
		for (auto it = allcomname.begin(); it != allcomname.end(); it++)
		{
			bool opened = ImGui::MenuItem(it->c_str());
			if (opened)
			{
				if (*it == RenderComponent::RTTI().GetName())
				{
					M_CMD_EXEC_BEGIN;
					MCommandManager::Instance()->Execute<MCmdCreateRenderComponent>(node->GetStaticID(), *it);
					M_CMD_EXEC_END;

				}
				else
				{
					M_CMD_EXEC_BEGIN;
					MCommandManager::Instance()->Execute<MCmdCreateComponent>(node->GetStaticID(), *it);
					M_CMD_EXEC_END;
				}
			}
		}
		ImGui::EndPopup();
	}
}

NS_JYE_END