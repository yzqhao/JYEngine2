#include "MPRenderComponent.h"
#include "Engine/component/render/RenderComponent.h"
#include "InspectorUtil.h"
#include "../../../system/EditorSystem.h"
#include "../../../command/MCommandManager.h"
#include "../../../command/commands/MCmdPropertyChange.h"
#include "../../../command/commands/node/MCmdDeleteComponent.h" 
#include "../../../command/commands/rendercomponent/MCmdAddRenderProperty.h"
#include "../../../command/commands/rendercomponent/MCmdRemoveRenderProperty.h"
#include "../../../system/MDefined.h"
#include "../../selectwindow/MSelectWindow.h"
#include "../../../command/commands/rendercomponent/MCmdAddMaterial.h"
#include "../../../command/commands/rendercomponent/MCmdRemoveMaterial.h"
#include "../../../command/commands/rendercomponent/MCmdChangeMesh.h"
#include "../../../command/commands/rendercomponent/MCmdSetParameter.h"
#include "../../../command/commands/rendercomponent/MCmdChangeMaterial.h"
#include "../../../command/commands/rendercomponent/MCmdChangeShader.h"
#include "Engine/render/material/IMaterialSystem.h"
#include "Engine/render/material/DefProperty.h"
#include "Engine/Engine.h"
#include "Engine/render/RenderObjectEntity.h"
#include "util/MFileUtility.h"

#include "imgui.h"
#include "imgui_internal.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(MPRenderComponent, MPanelBase);
BEGIN_ADD_PROPERTY(MPRenderComponent, MPanelBase);
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(MPRenderComponent)
IMPLEMENT_INITIAL_END

const static Map<GraphicDefine::RenderProperty, String> g_render_property =
{
	{GraphicDefine::RP_SHOW, "Show"},
	{GraphicDefine::RP_CULL, "Camera cull"},
	{GraphicDefine::RP_LIGHT, "Lighting"},
	{GraphicDefine::RP_SHADOW_CASTER, "Cast shadow"},
	{GraphicDefine::RP_SHADOW_RECEIVER, "Receive shadow"},
	{GraphicDefine::RP_IGNORE_PICK, "Ignore camera ray pick"},
};

static bool IsDefaultMat(const String& matname)
{
	return false;
}

MPRenderComponent::MPRenderComponent()
	: MPanelBase(RenderComponent::RTTI().GetName())
{

}

MPRenderComponent::~MPRenderComponent()
{

}

static void InitMaterialFromMat(RenderComponent* ren, const String& matpath, int idx)
{
	uint64 StaticID = ren->GetStaticID();
	int matcnt = ren->GetMaterialCount();
	if (idx == matcnt)
	{
		M_CMD_EXEC_BEGIN;
		MCommandManager::Instance()->Execute<MCmdAddMaterial>(StaticID, matpath, idx);
		M_CMD_EXEC_END;
	}
	else
	{
		M_CMD_EXEC_BEGIN;
		MCommandManager::Instance()->Execute<MCmdChangeMaterial>(StaticID, matpath, idx);
		M_CMD_EXEC_END;
	}
}

bool MPRenderComponent::OnGui(Component* com, float dt)
{
	if (!EditorSystem::Instance()->ShowEverything() && com->GetEditorUIType() == Component::UNVISUAL)
		return false;

	RenderComponent* ren = static_cast<RenderComponent*>(com);
	uint64 StaticID = com->GetStaticID();

	uint64 hash = (ren->GetObjectID());
	String strhash = std::to_string(hash);
	String label = "RenderComponent##%s" + strhash;
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
		if (EditorSystem::Instance()->ShowEverything())
		{
			ImGui::Text("EditorUIType");
			ImGui::SameLine();
			Animatable::EditorUIType uiType = com->GetEditorUIType();
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

		ImGui::Text("Render Order  ");
		ImGui::SameLine();
		int renderOrder = ren->GetRenderOrder();
		bool orderischange = ImGui::InputInt(("##Render Order" + strhash).c_str(), &renderOrder);
		if (orderischange)
		{
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdPropertyChangeint>(StaticID, "m_RenderOrder", renderOrder);
			M_CMD_EXEC_END;
		}

		if (ImGui::TreeNode("Render property"))
		{
			for (auto& it : g_render_property)
			{
				bool rp = ren->isRenderProperty(it.first);
				bool rp_change = ImGui::Checkbox(it.second.c_str(), &rp);
				if (rp_change)
				{
					if (rp)
					{
						M_CMD_EXEC_BEGIN;
						MCommandManager::Instance()->Execute<MCmdAddRenderProperty>(StaticID, it.first);
						M_CMD_EXEC_END;
					}
					else
					{
						M_CMD_EXEC_BEGIN;
						MCommandManager::Instance()->Execute<MCmdRemoveRenderProperty>(StaticID, it.first);
						M_CMD_EXEC_END;
					}
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::TreePush("Mesh");
			ImGui::Text("Mesh");
			ImGui::SameLine();
			Math::IntVec4 size = Engine::Instance()->GetMainViewSize();
			ImVec2 btnSize = ImVec2(size.z / 8.0, size.w / 45.0);
			ImVec2 changeBtnSize = ImVec2(size.z / 16.0, size.w / 45.0);
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4 gray = style.Colors[ImGuiCol_FrameBg];
			ImGui::PushStyleColor(ImGuiCol_Button, gray);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, gray);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, gray);
			ImGui::Button(ren->GetRenderObjectEntity()->GetMeshName().c_str(), btnSize);
			ImGui::PopStyleColor(3);
			ImGui::SameLine();
			if (ImGui::Button("Change", changeBtnSize))
			{
				g_SeclectWindow->Open({ "mesh", "dynamicmesh" }, MSelectWindow::SelectDelegate::CreateLambda([&, StaticID](const String& selectPath, Object* obj) {
					if (MFileUtility::getExtension(selectPath) == "mesh")
					{
						RenderObjectMeshFileMetadate meta(RHIDefine::MU_STATIC, selectPath);
						M_CMD_EXEC_BEGIN;
						MCommandManager::Instance()->Execute<MCmdChangeMesh>(StaticID, meta);
						M_CMD_EXEC_END;
					}
					else if (MFileUtility::getExtension(selectPath) == "dynamicmesh")
					{
						RenderObjectMeshFileMetadate meta(RHIDefine::MU_DYNAMIC, selectPath);
						M_CMD_EXEC_BEGIN;
						MCommandManager::Instance()->Execute<MCmdChangeMesh>(StaticID, meta);
						M_CMD_EXEC_END;
					}
					}), true, { "comm:mesh/basicobjects" });
			}
		}

		const Vector<MaterialEntity*>& materials = ren->GetMaterialEntities();
		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::TreePush("Material");
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 6.5);
			float ButtonWidth = ImGui::GetWindowWidth() / 1.4;
			ImVec2 btnSize = ImVec2(ButtonWidth, 0.0);

			if (ImGui::Button("add material", btnSize))
			{
				Vector<String> matExtraDir = { "comm:/mat" };
				g_SeclectWindow->Open({ "mat" }, MSelectWindow::SelectDelegate::CreateLambda([&, ren, materials](const String& path, Object* obj) {
					InitMaterialFromMat(ren, path, materials.size());
					}), true, matExtraDir);
			}
			for (int i = 0; i < materials.size(); ++i)
			{
				ImGui::Text("Element%d", i);
				ImGui::SameLine();
				String matName = materials[i]->GetMatPath();
				if (ImGui::Button(matName.c_str()))
				{
					Vector<String> matExtraDir = { "comm:/mat" };
					g_SeclectWindow->Open({ "mat" }, MSelectWindow::SelectDelegate::CreateLambda([&, ren, materials, i](const String& path, Object* obj) {
						InitMaterialFromMat(ren, path, i);
						}), true, matExtraDir);
				}
			}
			ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 6.5);
			bool isdelete = ImGui::Button("delete material", btnSize);
			if (isdelete)
			{
				int cursize = materials.size();
				M_CMD_EXEC_BEGIN;
				MCommandManager::Instance()->Execute<MCmdRemoveMaterial>(StaticID, cursize);
				M_CMD_EXEC_END;
			}

			ImGui::TreePop();
		}

		const Vector<MaterialEntity*>& newmaterials = ren->GetMaterialEntities();
		for (int materialidx = 0; materialidx < newmaterials.size(); ++materialidx)
		{
			MaterialEntity* material = materials[materialidx];
			String hashidx = strhash + std::to_string(materialidx);
			String matName = materials[materialidx]->GetMatPath();
			String materialName = materials[materialidx]->GetShaderPath();
			String namehash = materialName + std::to_string(materialidx);
			if (ImGui::CollapsingHeader(namehash.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::TreePush(namehash.c_str());
				if (IsDefaultMat(matName))
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5);
				}
				ImGui::Text(("Material"+ std::to_string(materialidx)).c_str());
				ImGui::SameLine();
				if (ImGui::Button(materialName.c_str()))
					ImGui::OpenPopup(("Change Materials"+std::to_string(materialidx)).c_str());
				bool isMaterialChange = ImGui::BeginPopup(("Change Materials" + std::to_string(materialidx)).c_str());
				static bool preMaterialChangeState = false;
				Vector<String> searchPaths;
				if (preMaterialChangeState != isMaterialChange )
				{
					MFileUtility::GetFilesInDir(searchPaths, MDefined::ShaderPath, true, { "shader" });
				}
				if (isMaterialChange)
				{
					for (auto& str : searchPaths)
					{
						String relative = MFileUtility::GetRelativePath(str, { MDefined::ProjectPath, MDefined::ShaderPath });
						if (ImGui::MenuItem(relative.c_str()))
						{
							M_CMD_EXEC_BEGIN;
							MCommandManager::Instance()->Execute<MCmdChangeShader>(StaticID, relative, materialidx);
							M_CMD_EXEC_END;
						}
					}
					ImGui::EndPopup();
				}
				// 绘制 imgui 的组件 材质参数
				material = materials[materialidx];
				Map<String, MaterialParameter*> slots = ren->GetUniformParameter(materialidx);
				for (auto& it : slots)
				{
					String paramname = it.first;
					RHIDefine::ParameterSlot _Slot = IMaterialSystem::Instance()->GetParameterSlot(paramname);
					if (material->HasDefProperty(_Slot))	// TODO 处理隐藏
					{
						const DefProperty& _DefProperty = material->GetProperty(_Slot);
						ParserMaterial::ParamsType parseType = _DefProperty.GetParserType();
						String _desc = *_DefProperty.GetDefVal().pDescription;
						bool attrEnum = _DefProperty.IsEnum();
						bool attrKeyword = _DefProperty.IsKeyEnum();

						MaterialParameter* pparam = it.second;
						bool slotischange = false;
						MaterialParameter* slotnewvalue = nullptr;
						switch (parseType)
						{
						case ParserMaterial::VT_ERRORCODE:
							break;
						case ParserMaterial::VT_FLOAT:
							if (attrEnum == false && attrKeyword == false)
							{
								MaterialParameterFloat* param = static_cast<MaterialParameterFloat*>(pparam);
								slotischange = InspectorUtil::Vector1(param->m_float, paramname, 0.001, strhash + _desc, -100, 100, _desc);
								if (slotischange)
									slotnewvalue = new MaterialParameterFloat(param->m_float);
							}
							break;
						case ParserMaterial::VT_FLOATRANGE:
							break;
						case ParserMaterial::VT_VEC2:
						{
							MaterialParameterVec2* param = static_cast<MaterialParameterVec2*>(pparam);
							slotischange = InspectorUtil::Vector2(param->m_vec2, paramname, 0.001, strhash + _desc, 0.0f, 0.0f, _desc);
							if (slotischange)
								slotnewvalue = new MaterialParameterVec2(param->m_vec2);
						}
						break;
						case ParserMaterial::VT_VEC3:
						{
							MaterialParameterVec3* param = static_cast<MaterialParameterVec3*>(pparam);
							slotischange = InspectorUtil::Vector3(param->m_vec3, paramname, 0.001, strhash + _desc, 0.0f, 0.0f, _desc);
							if (slotischange)
								slotnewvalue = new MaterialParameterVec3(param->m_vec3);
						}
						break;
						case ParserMaterial::VT_VEC4:
						{
							MaterialParameterVec4* param = static_cast<MaterialParameterVec4*>(pparam);
							slotischange = InspectorUtil::Vector4(param->m_vec4, paramname, 0.001, strhash + _desc, _desc);
							if (slotischange)
								slotnewvalue = new MaterialParameterVec4(param->m_vec4);
						}
						break;
						case ParserMaterial::VT_COLOR:
						{
							MaterialParameterColor* param = static_cast<MaterialParameterColor*>(pparam);
							ImGui::Text(paramname.c_str());
							InspectorUtil::Hint(_desc);
							ImGui::SameLine();
							slotischange = ImGui::ColorEdit4((strhash+_desc).c_str(), param->m_color.GetPtr());
							if (slotischange)
								slotnewvalue = new MaterialParameterColor(param->m_color);
						}
						break;
						case ParserMaterial::VT_TEXTURE1D:
						case ParserMaterial::VT_TEXTURE2D:
						case ParserMaterial::VT_TEXTURE3D:
						case ParserMaterial::VT_TEXTURECUBE:
						{
							MaterialParameterTex* param = static_cast<MaterialParameterTex*>(pparam);
							String texname = param->GetTex()->GetName();
							RHIDefine::TextureType tt = param->GetTex()->GetTextureType();
							bool istouch = InspectorUtil::FileSelecter(_desc, texname, "change", param->GetTex(), std::to_string(materialidx * 100 + (int)_Slot),
								MSelectWindow::SelectDelegate::CreateLambda([&, StaticID, materialidx, _Slot](const String& selectPath, Object* obj) {
									TextureEntity* entity = static_cast<TextureEntity*>(obj);
									M_CMD_EXEC_BEGIN;
									MCommandManager::Instance()->Execute<MCmdSetParameter>(StaticID, materialidx, _Slot, new MaterialParameterTex(entity));
									M_CMD_EXEC_END;
									}), paramname, tt, {});
						}
						break;
						case ParserMaterial::VT_KEYWORDENUM:
							break;
						case ParserMaterial::VT_COUNT:
							break;
						default:
							break;
						}

						if (slotnewvalue)
						{
							M_CMD_EXEC_BEGIN;
							MCommandManager::Instance()->Execute<MCmdSetParameter>(StaticID, materialidx, _Slot, slotnewvalue);
							M_CMD_EXEC_END;
						}
					}
				}

				if (IsDefaultMat(matName))
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}
				ImGui::TreePop();
			}
		}

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 3);
		float ButtonWidth = ImGui::GetWindowWidth() / 3;
		if (ImGui::Button("Delete Component", ImVec2(ButtonWidth, 0.0)))
		{
			GObject* hostNode = ren->GetParentObject();
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdDeleteComponent>(StaticID, GetPanelName());
			M_CMD_EXEC_END;
		}

		ImGui::TreePop();
	}
	
	return true;
}

NS_JYE_END

