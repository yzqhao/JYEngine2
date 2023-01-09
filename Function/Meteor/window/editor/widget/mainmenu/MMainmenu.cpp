#include "MMainmenu.h"
#include "Core/Interface/IApplication.h"
#include "Core/Interface/IWindow.h"
#include "../../system/WindowSystem.h"
#include "../../system/EditorSystem.h"
#include "../../system/BundleManager.h"
#include "util/MSceneManagerExt.h"
#include "../selectwindow/MSelectScene.h"
#include "../selectwindow/MInputBox.h"

#include "imgui.h"
#include "Core/Interface/IFileSystem.h"
#include "../../command/MCommandManager.h"

NS_JYE_BEGIN

MMainmenu::MMainmenu()
	: m_curState(DoNothing)
	, m_closeAppState(CLOSEAPP_DoNothing)
	, m_selectSceneDialog(_NEW MSelectScene())
	, m_createSceneInpuBox(_NEW MInputBox("Create new scene", "newscene", MInputBox::CallFunc::CreateRaw(this, &MMainmenu::_OnNewSceneName)))
{
	m_windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking;
}

MMainmenu::~MMainmenu()
{

}

void MMainmenu::NewScene()
{
	m_curState = NewSceneOpenDiolog;
  	m_initDiologPos = true;
}

void MMainmenu::OpenScene()
{
	m_curState = OpenSceneOpenChoseWindow;
}

void MMainmenu::SaveScene()
{
	BundleManager::SaveScene(EditorSystem::Instance()->GetSceneFile());
}

void MMainmenu::SaveSceneAs()
{

}

void MMainmenu::ExportScene()
{
	m_curState = ExportSceneOpenWindow;
}

void MMainmenu::ShowFileMenu()
{
	if (ImGui::BeginMenu("File"))
	{
		if (m_focused)
		{
			if (ImGui::MenuItem("New Scene"))
			{
				NewScene();
			}
			if (ImGui::MenuItem("Open Scene"))
			{
				// OpenScene(); // TODO : 有bug，先屏蔽掉
			}
			if (ImGui::MenuItem("Save Scene"))
			{
				SaveScene();
			}
			if (ImGui::MenuItem("Export Scene"))
			{
				ExportScene();
			}
		}
		else
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndMenu();
	}

	if (m_curState == NewSceneOpenDiolog)
	{
		_OpenSaveDiolog();
	}
	else if (m_curState == NewSceneSaveScene)
	{
		SaveScene();
		m_curState = NewSceneDo;
	}
	else if (m_curState == NewSceneDo)
	{
		m_showCreateSceneInputBox = true;
		m_curState = DoNothing;
	}
	else if (m_curState == OpenSceneOpenChoseWindow)
	{
		m_showSelectSceneDialog = true;
		m_curState = OpenSceneGetPath;
	}
	else if (m_curState == OpenSceneGetPath)
	{
		if (m_selectSceneFile.empty())
		{
			m_curState = OpenSceneOpenDiolog;
			m_initDiologPos = true;
		}
	}
	else if (m_curState == OpenSceneOpenDiolog)
	{
		_OpenSaveDiolog();
	}
	else if (m_curState == OpenSceneLoadScene)
	{
		_LoadScene();
		m_selectSceneFile.clear();
		m_curState = DoNothing;
	}
	else if (m_curState == OpenSceneDoSave)
	{
		SaveScene();
		_LoadScene();
		m_selectSceneFile.clear();
		m_curState = DoNothing;
	}
	else if (m_curState == OpenSceneOpenSaveWindow)
	{
		m_curState = OpenSceneDoSave;
	}
	else if (m_curState == ExportSceneOpenWindow)
	{
		// TODO
		
	}
}

void MMainmenu::ShowEditMenu()
{
	if (ImGui::BeginMenu("Edit"))
	{
		if (m_focused)
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z", false, true))
			{
				MCommandManager::Instance()->Undo();
			}
			if (ImGui::MenuItem("Redo", "CTRL+Y", false, true))
			{
				MCommandManager::Instance()->Redo();
			}
			if (ImGui::MenuItem("Cut", "CTRL+X", false, true))
			{
			}
			if (ImGui::MenuItem("Copy", "CTRL+C", false, true))
			{
			}
			if (ImGui::MenuItem("Paste", "CTRL+V", false, true))
			{
			}
		}
		else
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndMenu();
	}
}

void MMainmenu::ShowDebugMenu()
{
	if (ImGui::BeginMenu("Debug"))
	{
		ImGui::EndMenu();
	}
}

void MMainmenu::ShowWindowsMenu()
{
	auto WindowList = WindowSystem::Instance()->GetMenuWindowList();
	if (ImGui::BeginMenu("Windows"))
	{
		if (m_focused)
		{
			for (auto& it : WindowList)
			{
				const String& titlename = it->GetTitleName();
				bool status = it->IsOnGui();
				bool bopen = (ImGui::MenuItem(titlename.c_str(), "", &status, true));
				if (bopen)
				{
					it->SetOnGui(status);
				}
			}
		}
		else
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndMenu();
	}
}

void MMainmenu::ShowExtraToolsMenu()
{
	if (ImGui::BeginMenu("ExtraTools"))
	{
		if (m_focused)
		{
			bool status = EditorSystem::Instance()->ShowEverything();
			bool bopen = ImGui::MenuItem("ShowHideItems", "", &status, true);
			if (bopen)
			{
				EditorSystem::Instance()->SetShowStaus(status);
			}
		}
		else
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndMenu();
	}
}

void MMainmenu::ShowCloseAppMenu()
{

}

bool MMainmenu::OnGui(float dt)
{
	bool result = false;
	if (ImGui::BeginMenuBar())
	{
		m_focused = IApplication::Instance()->GetMainWindow()->GetFocus();

		ShowFileMenu();

		ShowEditMenu();
		
		ShowWindowsMenu();

		ShowDebugMenu();

		ShowCloseAppMenu();

		ShowExtraToolsMenu();

		ImGui::EndMenuBar();
	}

	if (m_showSelectSceneDialog)
	{
		m_showSelectSceneDialog = false;
		m_selectSceneDialog->Open(MulticastDelegate<const String&>::DelegateT::CreateRaw(this, &MMainmenu::_OnSelectScene));
	}

	m_selectSceneDialog->Draw();

	if (m_showCreateSceneInputBox)
	{
		m_showCreateSceneInputBox = false;
		m_createSceneInpuBox->Open();
	}

	m_createSceneInpuBox->Draw();

	return result;
}

void MMainmenu::_OpenSaveDiolog()
{
	if (m_initDiologPos)
	{
		ImVec2 CursorScreenPos = ImGui::GetCursorScreenPos();
		ImGui::SetNextWindowPos(ImVec2(CursorScreenPos.x + 100, CursorScreenPos.y + 100));
		ImGui::SetNextWindowSize(ImVec2(170, 80), ImGuiCond_FirstUseEver);
		m_initDiologPos = false;
	}
	bool show = true;
	bool b1 = ImGui::Begin("Save current scene", &show, m_windowFlags);
	if (!show)
	{
		if (m_curState == OpenSceneOpenDiolog)
			m_curState = OpenSceneLoadScene;
		else
			m_curState = DoNothing;
	}
	ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
	float contentRegionAvailWidth = contentRegionAvail.x;
	float contentRegionAvailHeight = contentRegionAvail.y;
	ImVec2 rectSize = ImGui::GetItemRectSize();
	ImVec2 currentCursorPos = ImGui::GetCursorPos();
	ImGui::SetCursorPos(ImVec2(currentCursorPos.x + contentRegionAvailWidth / 6, currentCursorPos.y + contentRegionAvailHeight / 2));
	if (ImGui::Button("Save"))
	{
		if (m_curState == NewSceneOpenDiolog)
			m_curState = NewSceneSaveScene;
		else if (m_curState == OpenSceneOpenDiolog)
			m_curState = OpenSceneOpenSaveWindow;
	}
	ImGui::SameLine();
	if (ImGui::Button("Not Save"))
	{
		if (m_curState == NewSceneOpenDiolog)
			m_curState = NewSceneDo;
		else if (m_curState == OpenSceneOpenDiolog)
			m_curState = OpenSceneLoadScene;
	}
	ImGui::End();
}

void MMainmenu::_LoadScene()
{
	EditorSystem::Instance()->ClearScene();
	//Drag:DropObject();
	EditorSystem::Instance()->OpenScene(m_selectSceneFile, RawDelegate<false, MEditor, void()>());
}

void MMainmenu::_OnSelectScene(const String& scenePath)
{
	m_selectSceneFile.clear();
	if (!scenePath.empty())
		m_selectSceneFile = IFileSystem::Instance()->PathAssembly("proj:") + scenePath;
}

bool MMainmenu::_OnNewSceneName(const String& name, String& outError)
{
	auto path = IFileSystem::Instance()->PathAssembly("proj:assets/") + name + '/';
	if (IFileSystem::Instance()->isFileExist(path))
	{
		outError = "Scene already exists, please use another name.";	// "场景已存在，请使用其它名字。";
		return false;
	}

	EditorSystem::Instance()->ClearScene();
	EditorSystem::Instance()->CreateScene(path, name);
	//Drag:DropObject();
	return true;
}

NS_JYE_END