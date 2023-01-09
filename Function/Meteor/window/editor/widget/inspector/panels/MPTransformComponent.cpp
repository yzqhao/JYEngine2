#include "MPTransformComponent.h"
#include "Engine/component/transform/TransformComponent.h"
#include "../../../system/EditorSystem.h"
#include "../../../command/MCommandManager.h"
#include "../../../command/commands/MCmdPropertyChange.h"
#include "InspectorUtil.h"

#include "imgui.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(MPTransformComponent, MPanelBase);
BEGIN_ADD_PROPERTY(MPTransformComponent, MPanelBase);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(MPTransformComponent)
IMPLEMENT_INITIAL_END

MPTransformComponent::MPTransformComponent()
	: MPanelBase(TransformComponent::RTTI().GetName())
{

}

MPTransformComponent::~MPTransformComponent()
{

}

bool MPTransformComponent::OnGui(Component* com, float dt)
{
	if (!EditorSystem::Instance()->ShowEverything() && com->GetEditorUIType() == Component::UNVISUAL)
		return false;

	TransformComponent* trans = static_cast<TransformComponent*>(com);
	uint64 StaticID = com->GetStaticID();

	uint64 hash = (trans->GetObjectID());
	String strhash = std::to_string(hash);
	String label = "Transform##%s" + strhash;
	if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TreePush(label.c_str());

		Math::Vec3 pos = trans->GetLocalPosition();
		bool posischange = InspectorUtil::Vector3(pos, "Position  ", 1.0, strhash);
		if (posischange)
		{
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdPropertyChangeVec3>(StaticID, "m_LocalPosition", pos);
			M_CMD_EXEC_END;
		}

		Math::Vec3 scale = trans->GetLocalScale();
		bool scaleischange = InspectorUtil::Vector3(scale, "Scale     ", 1.0, strhash);
		if (scaleischange)
		{
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdPropertyChangeVec3>(StaticID, "m_LocalScale", scale);
			M_CMD_EXEC_END;
		}


		Math::Vec3 eularangle = trans->GetLocalEularAngle(); 
		bool roischange = InspectorUtil::EularAngleInspect(eularangle, "Rotation  ", 0.1, strhash);
		if (roischange)
		{
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdPropertyChangeVec3>(StaticID, "m_LocalEularAngle", eularangle);
			M_CMD_EXEC_END;
		}

		ImGui::Text("Quaternion");
		ImGui::SameLine();
		Math::Quaternion q = trans->GetLocalRotation();
		ImGui::InputFloat4(("##" + GetPanelName() + strhash).c_str(), (float*)q.GetPtr(), "%.2f", ImGuiInputTextFlags_ReadOnly);

		ImGui::TreePop();
	}
	
	return true;
}

NS_JYE_END

