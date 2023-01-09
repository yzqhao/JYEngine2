#include "MProject.h"
#include "MProjectManager.h"
#include "Core/Interface/IApplication.h"
#include "Core/Interface/IWindow.h"
#include "util/MeteorUtil.h"
#include "window/editor/widget/selectwindow/MSelectScene.h"
#include "window/editor/widget/selectwindow/MInputBox.h"
#include "System/Delegate.h"

#include "imgui.h"

NS_JYE_BEGIN

const static int dw = 800;
const static int dh = 600;

MProject::MProject()
	: m_selectScene(_NEW MSelectScene())
	, m_createScene(_NEW MInputBox("Create new scene", "newscene", MInputBox::CallFunc::CreateRaw(MProjectManager::Instance(), &MProjectManager::CreateScene)))
	, m_opend(false)
	, m_isOpenDialog(false)
{

}

MProject::~MProject()
{

}

void MProject::Show()
{
	m_opend = true;
}

bool MProject::DrawStartPage()
{
	const String& projectPath = MProjectManager::Instance()->GetProjectPath();

	ImGui::AlignTextToFramePadding();
	if (!projectPath.empty()) 
		ImGui::TextUnformatted(projectPath.c_str());
	else
		ImGui::TextUnformatted("Please select a project folder.");

	ImGui::SameLine();

	if (ImGui::Button("Select Project"))
	{
		MeteorUtil::OpenFloderDialog(IApplication::Instance()->GetMainWindow(), String(""));
		m_isOpenDialog = true;
	}
	
	if (m_isOpenDialog)
	{
		Vector<String> selectedPath;
		MeteorUtil::GetSelectedPath(IApplication::Instance()->GetMainWindow(), selectedPath);
		if (!selectedPath.empty())
		{
			MProjectManager::Instance()->CreateProject(selectedPath[0]);
		}
	}

	if (!projectPath.empty())
	{
		ImGui::SameLine();

		if (ImGui::Button("Select Scene"))
		{
			m_selectScene->Open(MulticastDelegate<const String&>::DelegateT::CreateRaw(MProjectManager::Instance(), & MProjectManager::SelectScene));
		}

		ImGui::SameLine();

		if (ImGui::Button("New Scene"))
		{
			m_createScene->Open();
		}

		m_selectScene->Draw();
		m_createScene->Draw();
	}

	return !MProjectManager::Instance()->GetSceneFullName().empty();
}

MWindowIndex MProject::Update(float dt)
{
	ImGui::SetNextWindowSize(ImVec2(dw, dh), ImGuiCond_FirstUseEver);

	ImGui::Begin("Project", &m_opend, ImGuiWindowFlags_MenuBar);
	if (!m_opend)
	{
		IApplication::Instance()->Abort();
		ImGui::End();
	}

	if (DrawStartPage())
	{
		return MWI_Editor;
	}

	ImGui::End();

	return MWI_Project;
}

void MProject::Close()
{
	m_opend = false;
}

NS_JYE_END