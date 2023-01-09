#include "MSelectScene.h"
#include "Core/Interface/IFileSystem.h"
#include "util/MFileUtility.h"

#include "imgui.h"

NS_JYE_BEGIN

MSelectScene::MSelectScene()
	: m_open(false)
	, m_title("Select Scene")
{

}

MSelectScene::~MSelectScene()
{

}

void MSelectScene::Open(CallbackDelegate& func)
{
	if (!m_open)
	{
		m_open = true;
		String& projPath = IFileSystem::Instance()->PathAssembly("proj:");
		m_searchScenes.clear();
		MFileUtility::SearchFiles(projPath, {"scene"}, m_searchScenes);
		m_callFunc.Add(std::move(func));
		ImGui::OpenPopup(m_title.c_str());
	}
}

void MSelectScene::Draw()
{
	ImGui::SetNextWindowSize(ImVec2(600, 500));
	bool b_modal = true;
	if (ImGui::BeginPopupModal(m_title.c_str(), &b_modal, ImGuiWindowFlags_NoResize))
	{
		if (m_open)
		{
			for (int i = 0; i < m_searchScenes.size(); ++i)
			{
				if (ImGui::Selectable(m_searchScenes[i].c_str()))
				{
					m_callFunc.Broadcast(m_searchScenes[i]);
					Close();
				}
			}
		}
		ImGui::EndPopup();
	}
	else if (m_open)
	{
		m_callFunc.Broadcast("");
		Close();
	}
}

void MSelectScene::Close()
{
	m_open = false;
	m_callFunc.Clear();
	ImGui::CloseCurrentPopup();
}

NS_JYE_END