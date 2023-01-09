#include "MPCameraComponent.h"
#include "Engine/component/camera/CameraComponent.h"
#include "InspectorUtil.h"
#include "../../../system/EditorSystem.h"
#include "../../../command/MCommandManager.h"
#include "../../../command/commands/MCmdPropertyChange.h"
#include "../../../command/commands/node/MCmdDeleteComponent.h"

#include "imgui.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(MPCameraComponent, MPanelBase);
BEGIN_ADD_PROPERTY(MPCameraComponent, MPanelBase);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(MPCameraComponent)
IMPLEMENT_INITIAL_END

MPCameraComponent::MPCameraComponent()
	: MPanelBase(CameraComponent::RTTI().GetName())
{

}

MPCameraComponent::~MPCameraComponent()
{

}

bool MPCameraComponent::OnGui(Component* com, float dt)
{
	if (!EditorSystem::Instance()->ShowEverything() && com->GetEditorUIType() == Component::UNVISUAL)
		return false;

	CameraComponent* cam = static_cast<CameraComponent*>(com);
	uint64 StaticID = com->GetStaticID();

	uint64 hash = (cam->GetObjectID());
	String strhash = std::to_string(hash);
	String label = "CameraComponent##%s" + strhash;
	if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TreePush(label.c_str());

		bool isactive = com->isActive();
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

		float near = cam->GetNear();
		bool nearischange = InspectorUtil::Vector1(near, "Near      ", 0.5, "near", -10000, 10000);
		if (nearischange)
		{
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdPropertyChangefloat>(StaticID, "m_Near", near);
			M_CMD_EXEC_END;
		}

		float far = cam->GetFar();
		bool farischange = InspectorUtil::Vector1(far, "Far       ", 1, "far", -10000, 10000);
		if (farischange)
		{
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdPropertyChangefloat>(StaticID, "m_Far", far);
			M_CMD_EXEC_END;
		}

		ImGui::Text("Sequence  ");
		ImGui::SameLine();
		int sequence = cam->GetSequence();
		bool seqischange = ImGui::InputInt(("##sequence"+ strhash).c_str(), &sequence);
		if (seqischange)
		{
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdPropertyChangeint>(StaticID, "m_Sequence", sequence);
			M_CMD_EXEC_END;
		}

		Vector<String> comboItems = { "perspective", "orthographi", "realcamera" };
		String currentstatus = comboItems[cam->GetProjType()];
		ImGui::Text("Projection");
		ImGui::SameLine();
		bool iscombo = ImGui::BeginCombo("Projection", currentstatus.c_str());
		if (iscombo)
		{
			for (int i = 0; i < comboItems.size(); ++i)
			{
				bool isselected = (currentstatus == comboItems[i]);
				if (ImGui::Selectable(comboItems[i].c_str(), &isselected))
				{
					M_CMD_EXEC_BEGIN;
					MCommandManager::Instance()->Execute<MCmdPropertyChangeint>(StaticID, "m_ProjectionType", i);
					if (cam->GetProjType() == GraphicDefine::CameraProjectionType::CPT_PERSPECTIVE)
					{
						MCommandManager::Instance()->Execute<MCmdPropertyChangefloat>(StaticID, "m_Fov_Size", 180 / 4);
					}
					else
					{
						MCommandManager::Instance()->Execute<MCmdPropertyChangefloat>(StaticID, "m_Fov_Size", 1.0);
					}
					M_CMD_EXEC_END;
				}
				if (isselected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (cam->GetProjType() == GraphicDefine::CameraProjectionType::CPT_PERSPECTIVE)
		{
			float fov = cam->GetFov() / Math::PI * 180.0f;
			bool fovischange = InspectorUtil::Vector1(fov, "Fov       ", 0.5, "fov", 0, 180);
			if (fovischange)
			{
				M_CMD_EXEC_BEGIN;
				MCommandManager::Instance()->Execute<MCmdPropertyChangefloat>(StaticID, "m_Fov_Size", fov * Math::PI / 180.0f);
				M_CMD_EXEC_END;
			}
		}
		else if (cam->GetProjType() == GraphicDefine::CameraProjectionType::CPT_ORTHOGRAPHI)
		{
			float size = cam->GetFov();
			bool fovischange = InspectorUtil::Vector1(size, "Size      ", 0.5, "size", 0, 1000);
			if (fovischange)
			{
				M_CMD_EXEC_BEGIN;
				MCommandManager::Instance()->Execute<MCmdPropertyChangefloat>(StaticID, "m_Fov_Size", size);
				M_CMD_EXEC_END;
			}
		}

		ImGui::Text("ClearColor");
		ImGui::SameLine();
		Math::FLinearColor clearcolor = cam->GetClearColor();
		bool colorischange = ImGui::ColorEdit4(" ", clearcolor.GetPtr());
		if (colorischange)
		{
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdPropertyChangeFLinearColor>(StaticID, "m_ClearColor", clearcolor);
			M_CMD_EXEC_END;
		}

		if (ImGui::TreeNode("ClearFlags"))
		{
			Map<String, bool> allflagname = {};
			allflagname["color"] = false;
			allflagname["depth"] = false;
			allflagname["stencil"] = false;
			Map<String, RHIDefine::ClearFlag> allflags = {};
			allflags["color"] = RHIDefine::CF_COLOR;
			allflags["depth"] = RHIDefine::CF_DEPTH;
			allflags["stencil"] = RHIDefine::CF_STENCIL;
			RHIDefine::ClearFlag flag = cam->GetClearFlag();
			for (auto& it : allflagname)
			{
				if ((flag & allflags[it.first]) > 0)
				{
					it.second = true;
				}
			}
			for (auto& it : allflagname)
			{
				bool v = it.second;
				bool flagischange = ImGui::Checkbox(it.first.c_str(), &v);
				if (flagischange)
				{
					if (v)
					{
						int _ClearFlag = flag | allflags[it.first];
						M_CMD_EXEC_BEGIN;
						MCommandManager::Instance()->Execute<MCmdPropertyChangeint>(StaticID, "m_eClearFlag", _ClearFlag);
						M_CMD_EXEC_END;
					}
					else
					{
						int notv = (~allflags[it.first]);
						int _ClearFlag = flag & notv;
						M_CMD_EXEC_BEGIN;
						MCommandManager::Instance()->Execute<MCmdPropertyChangeint>(StaticID, "m_eClearFlag", _ClearFlag);
						M_CMD_EXEC_END;
					}
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Layer"))
		{
			ImGui::TreePop();
		}

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 3);
		float ButtonWidth = ImGui::GetWindowWidth() / 3;
		if (ImGui::Button("Delete Component", ImVec2(ButtonWidth, 0.0)))
		{
			GObject* hostNode = cam->GetParentObject();
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdDeleteComponent>(StaticID, GetPanelName());
			M_CMD_EXEC_END;
		}

		ImGui::TreePop();
	}

	return true;
}

NS_JYE_END

