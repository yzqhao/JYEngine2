
#include "MTexture.h"
#include "Core/Interface/IFile.h"
#include "Core/Interface/IFileSystem.h"
#include "Engine/resource/TextureMetadata.h"
#include "../../../../system/MDefined.h"


#include "imgui.h"

NS_JYE_BEGIN

static HashMap<String, TextureDescribeDataPtr> g_textureProperty;

void MTexture::OnGui(const String& assetspath)
{
	String path = IFileSystem::Instance()->PathAssembly(assetspath);
	String metaPath = path + ".meta";
	String strlabelhash = "texture##" + path;
	if (ImGui::CollapsingHeader(strlabelhash.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TreePush(strlabelhash.c_str());

		TextureDescribeDataPtr pTexDes;
		if (g_textureProperty.count(path))
		{
			pTexDes = g_textureProperty[path];
		}
		else
		{
			if (IFileSystem::Instance()->isFileExist(metaPath))
			{
				Stream stream;
				stream.Load(metaPath.c_str());
				pTexDes = _NEW TextureDescribeData(*(TextureDescribeData*)stream.GetObjectByRtti(TextureDescribeData::RTTI()));
			}
			else
			{
				pTexDes = _NEW TextureDescribeData();
			}
			g_textureProperty.insert({ path, pTexDes });
		}

		{	// TextureType
			RHIDefine::TextureType currentType = pTexDes->m_eType;
			String currentName = "";
			for (auto& it : MDefined::TextureTypeMap)
				if (currentType == it.second) 
					currentName = it.first;
			ImGui::Text("TextureType");
			ImGui::SameLine();
			if (ImGui::BeginCombo(("##TextureType" + strlabelhash).c_str(), currentName.c_str()))
			{
				for (auto& it : MDefined::TextureTypeMap)
				{
					bool isSelected = currentType == it.second;
					if (ImGui::Selectable(it.first.c_str(), &isSelected))
						pTexDes->m_eType = it.second;
				}
				ImGui::EndCombo();
			}
		}
		{	// SWrap
			RHIDefine::TextureWarp currentWrap = pTexDes->m_eSWarp;
			ImGui::Text("SWrap");
			ImGui::SameLine();
			if (ImGui::BeginCombo(("##SWrap" + strlabelhash).c_str(), MDefined::TextureWrapName[currentWrap].c_str()))
			{
				for (int i = 0; i < MDefined::TextureWrapName.size(); ++i)
				{
					bool isSelected = currentWrap == MDefined::TextureWrap[i];
					if (ImGui::Selectable(MDefined::TextureWrapName[i].c_str(), &isSelected))
						pTexDes->m_eSWarp = MDefined::TextureWrap[i];
				}
				ImGui::EndCombo();
			}
		}
		{	// TWrap
			RHIDefine::TextureWarp currentWrap = pTexDes->m_eTWarp;
			ImGui::Text("TWrap");
			ImGui::SameLine();
			if (ImGui::BeginCombo(("##TWrap" + strlabelhash).c_str(), MDefined::TextureWrapName[currentWrap].c_str()))
			{
				for (int i = 0; i < MDefined::TextureWrapName.size(); ++i)
				{
					bool isSelected = currentWrap == MDefined::TextureWrap[i];
					if (ImGui::Selectable(MDefined::TextureWrapName[i].c_str(), &isSelected))
						pTexDes->m_eTWarp = MDefined::TextureWrap[i];
				}
				ImGui::EndCombo();
			}
		}
		{	// MagFilter
			RHIDefine::TextureFilter current = pTexDes->m_eMagFilter;
			ImGui::Text("MagFilter");
			ImGui::SameLine();
			if (ImGui::BeginCombo(("##MagFilter" + strlabelhash).c_str(), MDefined::TextureFilterName[current].c_str()))
			{
				for (int i = 0; i < MDefined::TextureFilterName.size(); ++i)
				{
					bool isSelected = current == MDefined::TextureFilter[i];
					if (ImGui::Selectable(MDefined::TextureFilterName[i].c_str(), &isSelected))
						pTexDes->m_eMagFilter = MDefined::TextureFilter[i];
				}
				ImGui::EndCombo();
			}
		}
		{	// MinFilter
			RHIDefine::TextureFilter current = pTexDes->m_eMinFilter;
			ImGui::Text("MinFilter");
			ImGui::SameLine();
			if (ImGui::BeginCombo(("##MinFilter" + strlabelhash).c_str(), MDefined::TextureFilterName[current].c_str()))
			{
				for (int i = 0; i < MDefined::TextureFilterName.size(); ++i)
				{
					bool isSelected = current == MDefined::TextureFilter[i];
					if (ImGui::Selectable(MDefined::TextureFilterName[i].c_str(), &isSelected))
						pTexDes->m_eMinFilter = MDefined::TextureFilter[i];
				}
				ImGui::EndCombo();
			}
		}
		{	// IsMipMap
			ImGui::Text("IsMipMap");
			ImGui::SameLine();
			ImGui::Checkbox(("##IsMipMap" + strlabelhash).c_str(), &pTexDes->m_isMipMap);
		}
		{	// IsKeepSource
			ImGui::Text("IsKeepSource");
			ImGui::SameLine();
			ImGui::Checkbox(("##IsKeepSource" + strlabelhash).c_str(), &pTexDes->m_isKeepSource);
		}
		{	// sRGB
			ImGui::Text("sRGB");
			ImGui::SameLine();
			ImGui::Checkbox(("##sRGB" + strlabelhash).c_str(), &pTexDes->m_sRGB);
		}

		ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 3);
		float ButtonWidth = ImGui::GetWindowWidth() / 3;
		bool isapply = ImGui::Button("Apply", ImVec2(ButtonWidth, 0.0));
		if (isapply)
		{
			Stream stream;
			stream.SetStreamFlag(Stream::AT_REGISTER);
			stream.ArchiveAll(pTexDes);
			stream.Save(metaPath.c_str());
		}

		ImGui::TreePop();
	}
}

NS_JYE_END