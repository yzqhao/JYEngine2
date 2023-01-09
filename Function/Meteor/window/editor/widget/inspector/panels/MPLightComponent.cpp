#include "MPLightComponent.h"
#include "Engine/component/light/LightComponent.h"
#include "../../../system/EditorSystem.h"
#include "../../../command/MCommandManager.h"
#include "../../../command/commands/MCmdPropertyChange.h"
#include "../../../command/commands/node/MCmdDeleteComponent.h"
#include "InspectorUtil.h"

#include "imgui.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(MPLightComponent, MPanelBase);
BEGIN_ADD_PROPERTY(MPLightComponent, MPanelBase);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(MPLightComponent)
IMPLEMENT_INITIAL_END

MPLightComponent::MPLightComponent()
	: MPanelBase(LightComponent::RTTI().GetName())
{

}

MPLightComponent::~MPLightComponent()
{

}

bool MPLightComponent::OnGui(Component* com, float dt)
{
	if (!EditorSystem::Instance()->ShowEverything() && com->GetEditorUIType() == Component::UNVISUAL)
		return false;

	LightComponent* light = static_cast<LightComponent*>(com);
	uint64 StaticID = com->GetStaticID();

	uint64 hash = (light->GetObjectID());
	String strhash = std::to_string(hash);
	String label = "LightComponent##%s" + strhash;
	if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TreePush(label.c_str());

		Vector<String> comboItems = { "ambient", "directional", "point", "spot" };
		String currentlight = comboItems[light->GetLightType()];
		ImGui::Text("Type      ");
		ImGui::SameLine();
		bool iscombo = ImGui::BeginCombo(" ", currentlight.c_str());
		if (iscombo)
		{
			for (int i = 0; i < 4; ++i)
			{
				bool isselected = (currentlight == comboItems[i]);
				if (ImGui::Selectable(comboItems[i].c_str(), isselected))
				{
					M_CMD_EXEC_BEGIN;
					MCommandManager::Instance()->Execute<MCmdPropertyChangeint>(StaticID, "m_eLightType", i);
					M_CMD_EXEC_END;
				}
				if (isselected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (light->GetLightType() == GraphicDefine::LT_SPOT)
		{
			Math::Vec2 spotAngle = light->GetLightAngle() / Math::PI * 180;
			bool angelischange = InspectorUtil::Vector2(spotAngle, "spotAngle ", 0.05, "spotAngle" + strhash);
			if (angelischange)
			{
				M_CMD_EXEC_BEGIN;
				MCommandManager::Instance()->Execute<MCmdPropertyChangeVec2>(StaticID, "m_LightAngle", spotAngle * Math::PI / 180);
				M_CMD_EXEC_END;
			}
		}

		// ÏÔÊ¾ÒõÓ°
		if (light->GetLightType() == GraphicDefine::LT_DIRECTIONAL || light->GetLightType() == GraphicDefine::LT_SPOT)
		{
			Vector<String> shadowItems = { "none", "opaque" };
			ImGui::Text("Shadow    ");
			ImGui::SameLine();
			GraphicDefine::ShadowType shadowPreType = light->GetShadowType();
			String shadowComboCurrent = shadowItems[shadowPreType];
			bool iscombo = ImGui::BeginCombo("  ", shadowComboCurrent.c_str());
			if (iscombo)
			{
				for (int i = 0; i < shadowItems.size(); ++i)
				{
					bool isselected = (shadowComboCurrent == shadowItems[i]);
					if (ImGui::Selectable(shadowItems[i].c_str(), isselected))
					{
						GraphicDefine::ShadowType shadowType = (GraphicDefine::ShadowType)i;
						if (shadowType != shadowPreType)
						{
							M_CMD_EXEC_BEGIN;
							MCommandManager::Instance()->Execute<MCmdPropertyChangeint>(StaticID, "m_shadowType", shadowType);
							M_CMD_EXEC_END;
							shadowComboCurrent = shadowType;
						}
					}
					if (isselected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (light->GetShadowType() != GraphicDefine::NO_SHADOW)
			{
				if (ImGui::CollapsingHeader("Shadows",ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::TreePush(shadowComboCurrent.c_str());

					float bais = light->GetShadowType();
					bool baissichange = InspectorUtil::Vector1(bais, "shadow bais", 0.0001, "shadow bais", 0, 0.1);
					if (baissichange)
					{
						M_CMD_EXEC_BEGIN;
						MCommandManager::Instance()->Execute<MCmdPropertyChangefloat>(StaticID, "m_shadowBais", bais);
						M_CMD_EXEC_END;
					}

					float strength = light->GetShadowStrength();
					bool strengthchange = InspectorUtil::Vector1(strength, "shadow strength", 0.0001, "shadow strength", 0, 1);
					if (strengthchange)
					{
						M_CMD_EXEC_BEGIN;
						MCommandManager::Instance()->Execute<MCmdPropertyChangefloat>(StaticID, "m_shadowStrength", strength);
						M_CMD_EXEC_END;
					}

					ImGui::Text("Shadow Soft Type");
					ImGui::SameLine();
					Vector<String> allSoftTypes = { "PCF3x3", "PCF5x5", "PCF7x7", "PCFNoHardWare" };
					String st = allSoftTypes[light->GetSoftShadowType()];
					bool iscomboQa = ImGui::BeginCombo("      ", st.c_str());
					if (iscomboQa)
					{
						for (int i = 0; i < allSoftTypes.size(); ++i)
						{
							bool isselected = (st == allSoftTypes[i]);
							if (ImGui::Selectable(allSoftTypes[i].c_str(), isselected))
							{
								M_CMD_EXEC_BEGIN;
								MCommandManager::Instance()->Execute<MCmdPropertyChangeint>(StaticID, "m_softShadowType", i);
								M_CMD_EXEC_END;
							}
							if (isselected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				}
			}
		}

		if (light->GetLightType() == GraphicDefine::LT_SPOT || light->GetLightType() == GraphicDefine::LT_POINT)
		{
			float range = light->GetRange();
			bool ranischange = InspectorUtil::Vector1(range, "Range     ", 0.001, "Range" + strhash, 0, 100);
			if (ranischange)
			{
				M_CMD_EXEC_BEGIN;
				MCommandManager::Instance()->Execute<MCmdPropertyChangefloat>(StaticID, "m_LightRange", range);
				M_CMD_EXEC_END;
			}
			Math::Vec4 Attenuation = light->GetAttenuation();
			bool attischange = InspectorUtil::Vector4(Attenuation, "Attenuat  ", 0.001, "Attenuation" + strhash);
			if (attischange)
			{
				M_CMD_EXEC_BEGIN;
				MCommandManager::Instance()->Execute<MCmdPropertyChangeVec4>(StaticID, "m_Attenuation", Attenuation);
				M_CMD_EXEC_END;
			}
		}

		Math::Vec3 lightcolor = light->GetColor();
		ImGui::Text("color     ");
		ImGui::SameLine();
		bool isColorChange = ImGui::ColorEdit3(" ", lightcolor.GetPtr());
		if (isColorChange)
		{
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdPropertyChangeVec3>(StaticID, "m_Color", lightcolor);
			M_CMD_EXEC_END;
		}

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 3);
		float ButtonWidth = ImGui::GetWindowWidth() / 3;
		if (ImGui::Button("Delete Component", ImVec2(ButtonWidth, 0.0)))
		{
			GObject* hostNode = light->GetParentObject();
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdDeleteComponent>(StaticID, GetPanelName());
			M_CMD_EXEC_END;
		}

		ImGui::TreePop();
	}

	return true;
}

NS_JYE_END

