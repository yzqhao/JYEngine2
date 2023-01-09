#include "MAssets.h"
#include "window/project/MProjectManager.h"
#include "util/MFileUtility.h"
#include "Core/Interface/IFileSystem.h"
#include "../../system/MDefined.h"
#include "../../system/EditorSystem.h"
#include "util/MFileSearchTool.h"
#include "util/MeteorUtil.h"
#include "Core/Interface/IApplication.h"
#include "util/MImGuiUtil.h"
#include "Core/Interface/IWindow.h"
#include "FboTool.h"
#include "MatTool.h"
#include "System/Utility.hpp"
#include "util/MFileTree.h"
#include "Engine/object/GObject.h"
#include "../../command/MCommandManager.h"
#include "../../command/commands/node/MCmdSetNodePrefab.h"
#include "../../command/commands/node/MCmdSerializePrefab.h"

#include "imgui.h"
#include "imgui_internal.h"

#include <cstdlib>

NS_JYE_BEGIN

static bool openfiledialog = false;

static void __ShowInExplore(const String& assetpath)
{
#if _PLATFORM_WINDOWS
	String temp = assetpath;
	std::replace(temp.begin(), temp.end(), '/', '\\');
	String commandline = "explorer.exe /e,/root," + temp;
	system(commandline.c_str());
#else if _PLATFORM_MACOS
	//IApplication::Instance()->GetMainWindow()->ShowInFolder(assetpath, false);
#endif
}

MAssets::MAssets()
	: MMemuWindowBase("assets")
	, m_hasdropdown(false)
	, fbxdialog(false)
	, objDialog(false)
	, fbodialog(false)
	, meshdialog(false)
	, matdialog(false)
	, frameanidialog(false)
	, showmetafile(false)
	, currentID(0)
{
	m_projectpath = MProjectManager::Instance()->GetProjectPath(); // 得到工程目录
	MFileUtility::GetCleanPathForDialog(m_projectpath, m_projectpath);
	MFileUtility::MCreateDirectory(m_projectpath);
	IFileSystem::Instance()->SetProjPath(m_projectpath);
	MFileUtility::MCreateDirectoryDefined("proj:");  // 创建工程默认文件夹
	IFileSystem::Instance()->SetAsstPath(IFileSystem::Instance()->PathAssembly("proj:assets/"));
	IFileSystem::Instance()->SetDocmPath(IFileSystem::Instance()->PathAssembly(MDefined::SystemDirectories["resource"]));
	//IFileSystem:Instance()->SetScriptPath(IFileSystem:Instance() : PathAssembly(SystemDirectories["script"]));
	//AddScriptPath();

	EditorSystem::Instance()->RegisterAssetsPathChange(RawDelegate<false, MAssets, void(StaticDelegate<void()>&)>(this, &MAssets::OnAssetsPathChange));
}

MAssets::~MAssets()
{

}

void MAssets::OnAssetsPathChange(StaticDelegate<void()>& event)
{
	GetFileSystemTree(event);
}


void MAssets::GetFileSystemTree(StaticDelegate<void()>& event)
{
	m_filesearching = true;
	MFileSearchTool::Instance()->GetFileTree(this, event);
}

bool MAssets::OnGui(float dt)
{
	if (IsOnGui())
	{
		uint dockFlag = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;
		bool b = ImGui::Begin(m_titleName.c_str(), &m_isOnGui, dockFlag);

		// 文件结构初始化
		if (m_filesearching)
		{
			ImGui::OpenPopup("Initializing");
			if (ImGui::BeginPopupModal("Initializing"))
			{
				MImGuiUtil::CircleLoadingBar();
				ImGui::Separator();
				ImGui::Text("Initializing...");
				ImGui::EndPopup();
			}
		}

		if (fbxdialog)
		{
			// TODO
		}
		if (objDialog)
		{
			// TODO
		}
		if (fbodialog)
		{
			fbodialog = false;
			Vector<String>& selectedpath = IApplication::Instance()->GetMainWindow()->GetSelectedPath();
			if (!selectedpath.empty())
			{
				FboTool::CreateFbo(selectedpath[0]);
			}
		}
		if (meshdialog)
		{
			// TODO
		}
		if (matdialog)
		{
			matdialog = false;
			Vector<String>& selectedpath = IApplication::Instance()->GetMainWindow()->GetSelectedPath();
			if (!selectedpath.empty())
			{
				MatTool::CreateMat(selectedpath[0]);
			}
		}
		if (frameanidialog)
		{
			// TODO
		}

		if (!m_filesearching)
		{
			ImGui::BeginChild("background");
			//右键菜单
			_CheckContextWindow(nullptr);
			m_hasdropdown = true;
			String assetpath = IFileSystem::Instance()->PathAssembly(MDefined::SystemDirectories["scene_asset"]);
			MFileItem* fi = g_FileTree->GetNodeByID(Utility::HashCode(assetpath));
			_Showdirectory(fi);

			ImGui::EndChild();
		}

		ImGui::End();
	}
	return true;
}

void MAssets::CheckDrag(const String& path)
{
	if (_ProcessDragDropSource(path) == false)
		_ProcessDragDropTarget(path);
}

void MAssets::_CheckContextWindow(const String* filename)
{
	IWindow* mainWnd = IApplication::Instance()->GetMainWindow();
	String assetpath = IFileSystem::Instance()->PathAssembly(MDefined::SystemDirectories["scene_asset"]);
	if (filename)
	{
		if (ImGui::BeginPopupContextItem("asset right click", 1))
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Animation"))
				{
					MeteorUtil::OpenSaveFileDialog(mainWnd, "ani", assetpath, String("newanimation"));
					openfiledialog = true;
				}
				if (ImGui::MenuItem("Fbo"))
				{
					MeteorUtil::OpenSaveFileDialog(mainWnd, "fbo", assetpath, String("newfbo"));
					fbodialog = true;
				}
				if (ImGui::MenuItem("Mesh"))
				{
					MeteorUtil::OpenSaveFileDialog(mainWnd, "dynamicmesh", assetpath, String("newmesh"));
					meshdialog = true;
				}
				if (ImGui::MenuItem("Mat"))
				{
					MeteorUtil::OpenSaveFileDialog(mainWnd, "mat", assetpath, String("newmat"));
					matdialog = true;
				}
				if (ImGui::MenuItem("FrameAnimation"))
				{
					MeteorUtil::OpenMultiFileDialog(mainWnd, "png,jpg,jpeg", assetpath);
					frameanidialog = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Export", "", false, true))
			{
				//GlobalExportWindow:Open(node);
			}
			if (ImGui::BeginMenu("Import"))
			{
				if (ImGui::MenuItem("Fbx"))
				{
					//importfbxpath = filelfs::GetCurrentDirectory(*filename);
					MeteorUtil::OpenFileDialog(mainWnd, "fbx", String(""));
					fbxdialog = true;
				}
				if (ImGui::MenuItem("Obj"))
				{
					//importObjPath = filelfs:GetCurrentDirectory(*filename);
					MeteorUtil::OpenFileDialog(mainWnd, "obj", String(""));
					objDialog = true;
				}
				if (ImGui::MenuItem("Package"))
				{
					//GlobalImportWindow:Open(SystemDirectories["scene_asset"], self.projectpath);
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Delete"))
			{
				/*
				local absPath = node
				local postfix = filelfs:getExtension(absPath)
				local deleteSafely = true

				if (deleteSafely)
				{
					//os.remove(node);
					//os.remove(node .. ".meta");
				}
				*/
			}
			if (ImGui::MenuItem("Show in Explore"))
			{
				__ShowInExplore(*filename);
			}
			if (ImGui::MenuItem("Refresh"))
			{
				//self:GetFileSystemTree();
			}

			ImGui::EndPopup();
		}
	}
	else
	{
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Animation"))
				{
					MeteorUtil::OpenSaveFileDialog(mainWnd, "ani", assetpath, "newanimation");
					openfiledialog = true;
				}
				if (ImGui::MenuItem("Fbo"))
				{
					MeteorUtil::OpenSaveFileDialog(mainWnd, "fbo", assetpath, "newfbo");
					fbodialog = true;
				}
				if (ImGui::MenuItem("Mesh"))
				{
					MeteorUtil::OpenSaveFileDialog(mainWnd, "dynamicmesh", assetpath, "newmesh");
					meshdialog = true;
				}
				if (ImGui::MenuItem("Mat"))
				{
					MeteorUtil::OpenSaveFileDialog(mainWnd, "mat", assetpath, "newmat");
					matdialog = true;
				}
				if (ImGui::MenuItem("FrameAnimation"))
				{
					MeteorUtil::OpenMultiFileDialog(mainWnd, "png,jpg,jpeg", assetpath);
					frameanidialog = true;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Import"))
			{
				if (ImGui::MenuItem("Fbx"))
				{
					importfbxpath = assetpath;
					MeteorUtil::OpenFileDialog(mainWnd, "fbx", String(""));
					fbxdialog = true;
				}
				if (ImGui::MenuItem("Obj"))
				{
					importObjPath = assetpath;
					MeteorUtil::OpenFileDialog(mainWnd, "obj", String(""));
					objDialog = true;
				}
				if (ImGui::MenuItem("Package"))
				{
					//GlobalImportWindow::Open(SystemDirectories["scene_asset"], self.projectpath);
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Show in Explore"))
			{
				__ShowInExplore(assetpath);
			}
			if (ImGui::MenuItem("Refresh"))
			{
				//GetFileSystemTree();
			}

			ImGui::EndPopup();
		}
	}
}

void MAssets::_Showdirectory(MFileItem* fi)
{
	const auto& child = fi->GetChildren();
	for (int i = 0; i < child.size(); ++i)
	{
		const String& filename = child[i]->GetName();
		String filetype = MFileUtility::getExtension(filename);
		if (filetype != "meta" || showmetafile)
		{
			ImGui::PushID(filename.c_str());
			ImGui::AlignTextToFramePadding();
			bool is_selected = false;  // 标识选中状态
			String* selected = EditorSystem::Instance()->GetSelectedString();
			if (selected && *selected == filename)
				is_selected = true;
			String nodeName;
			MFileUtility::getFileNameWithPosfix(filename, nodeName);
			int nodeFlags = MDefined::TreeNodeFlag + ImGuiTreeNodeFlags_NoAutoOpenOnLog;
			if (is_selected)
				nodeFlags = nodeFlags + ImGuiTreeNodeFlags_Selected;

			bool no_children = child[i]->GetChildren().empty() ? true : false;
			if (no_children) // 没有子Node, 则创建叶子节点
				nodeFlags = nodeFlags + ImGuiTreeNodeFlags_Leaf;

			ImGui::AlignTextToFramePadding();
			bool opened = ImGui::TreeNodeEx(nodeName.c_str(), nodeFlags);
			CheckDrag(filename);  //处理拖拽
			if (ImGui::IsItemHovered() && ImGui::IsMouseDragging() == false)
			{
				if (ImGui::IsMouseClicked(0))
					currentID = ImGui::GetID(filename.c_str());
				uint localID = ImGui::GetID(filename.c_str());

				if (ImGui::IsMouseReleased(0) && currentID == localID)
					EditorSystem::Instance()->Select(filename); //选中此node
			}
			//在node上右键
			_CheckContextWindow(&filename);

			if (opened == true)
			{
				if (no_children == false)  //迭代子节点
					_Showdirectory(child[i]);
				ImGui::TreePop();
			}

			ImGui::PopID();
		}
	}
}

bool MAssets::_ProcessDragDropSource(const String& path)
{
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::TextUnformatted(path.c_str());  // 拖拽时显示node名称
		ImGui::SetDragDropPayload("assets", path.c_str(), path.size());  // 将ID传递过去，之后再通过ID查找NODE
		ImGui::EndDragDropSource();
		return true;
	}

	return false;
}

void MAssets::_ProcessDragDropTarget(const String& name)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (ImGui::IsDragDropPayloadBeingAccepted())
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		else
			ImGui::SetMouseCursor(ImGuiMouseCursor_COUNT); //禁止拖拽
		const ImGuiPayload* payloadAtapter = ImGui::AcceptDragDropPayload("Node");
		if (payloadAtapter)
		{
			String strid((char*)payloadAtapter->Data, payloadAtapter->DataSize);
			uint64 dragNodeID = std::stoull(strid);
			//由ID查找node对象
			GObject* dropNode = EditorSystem::Instance()->GetNodeByID(dragNodeID);	
			if (dropNode)
			{
				bool isPrefabChild = false;
				if (!isPrefabChild)
				{
					String nodename = dropNode->GetName();
					std::replace(nodename.begin(), nodename.end(), '.', '_');
					std::replace(nodename.begin(), nodename.end(), '/', '_');
					String currentDir = MFileUtility::MGetCurrentDirectory(name);
					String targetpath = IFileSystem::Instance()->PathAssembly(currentDir +  "/" + nodename + ".prefab");
					m_hasdropdown = true;
					String realTargetPath;
					MFileUtility::GetNoRepeatFileName(targetpath, realTargetPath);
					
					M_CMD_EXEC_BEGIN;
					String relativePath = MFileUtility::GetRelativePath(realTargetPath, {});
					MCommandManager::Instance()->Execute<MCmdSetNodePrefab>(dropNode->GetStaticID(), relativePath);
					MCommandManager::Instance()->Execute<MCmdSerializePrefab>(dropNode->GetStaticID(), relativePath);
					M_CMD_EXEC_END;
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}

NS_JYE_END