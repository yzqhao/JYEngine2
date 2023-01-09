#include "MSelectWindow.h"
#include "imgui.h"
#include "util/MSceneManagerExt.h"
#include "Engine/render/RenderTargetEntity.h"
#include "Engine/object/Scene.h"
#include "../inspector/panels/InspectorUtil.h"
#include "util/MFileTree.h"
#include "util/MFileUtility.h"
#include "Core/Interface/IFileSystem.h"
#include "../../system/MDefined.h"
#include "../../system/EditorSystem.h"
#include "Engine/resource/TextureMetadata.h"

NS_JYE_BEGIN

MSelectWindow::MSelectWindow()
	: MMemuWindowBase("Select")
	, m_open(false)
	, m_br(false)
	, m_window_flags(ImGuiWindowFlags_MenuBar)
	, m_showFolder(false)
	, m_type_rtti(nullptr)
	, m_hasInit(false)
{

}

MSelectWindow::~MSelectWindow()
{

}

void MSelectWindow::Open(const Vector<String>& typefilter, SelectDelegate& selectFunc, bool showFolder, const Vector<String>& extraPath)
{
	Open(typefilter, selectFunc, showFolder, extraPath, CloseDelegate(), "Select");
}

void MSelectWindow::Open(const Vector<String>& typefilter, SelectDelegate& selectCallBack, bool showFolder, const Vector<String>& extraPath, CloseDelegate& closeCallBack, const String& titleName)
{
	m_titleName = titleName;
	m_typefilter = typefilter;
	m_selectCallBack = std::move(selectCallBack);
	m_closeCallBack = std::move(closeCallBack);
	m_open = true;
	m_showFolder = showFolder;
	//自定义选择资源的目标路径
	m_targetPath = IFileSystem::Instance()->PathAssembly(MDefined::SystemDirectories["scene_asset"]);
	m_type_rtti = nullptr;
	m_extraPath = extraPath;
	//self.specialExtraFile = {};
	m_hasInit = false;
	if (showFolder)
	{
		m_filetree = { MFileUtility::GetDirectoryInFileTree(m_targetPath, typefilter) };
	}
}

void MSelectWindow::Close()
{
	m_open = false;
	m_namefilter = "";
	m_selectCallBack.Clear();
	m_typefilter = {};
	//m_serchFolder = {};
	m_hasInit = false;
	m_extraPath.clear();
	m_filetree.clear();
	//m_specialExtraFile = {};
}

bool MSelectWindow::OnGui(float dt)
{
	if (m_open)
	{
		bool close = true;
		bool b1 = ImGui::Begin(m_titleName.c_str(), &close, m_window_flags);
		if (!close)
			Close();

		int inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue + ImGuiInputTextFlags_AutoSelectAll;
		static char bufnodename[64] = {};
		strcpy(bufnodename, m_namefilter.c_str());
		m_br = ImGui::InputText("##MHierarchyScene_rename", bufnodename, 64, inputTextFlags);
		if (m_br)
			m_namefilter = String(bufnodename);

		ImGui::Separator();
		if (m_showFolder)
		{
			ShowFolderFilesByFilter(m_filetree, m_typefilter, m_namefilter, m_selectCallBack);
		}
		else
		{
			ShowFilesByFilter(g_FileTree->GetRootNode(), m_typefilter, m_namefilter, m_selectCallBack);
		}

		if (!m_warningText.empty())
		{
			InspectorUtil::ErrorView("Texture error", m_warningText, "Max size is 2048 x 2048", InspectorUtil::ErrorDelegate::CreateLambda([&, this]() {
				this->m_warningText.clear();
				}));
		}

		ImGui::End();
	}
	return true;
}

void MSelectWindow::ShowFilesByFilter(MFileItem* filetree, const Vector<String>& typefilter, const String& namefilter, SelectDelegate& callback)
{
	auto& children = filetree->GetChildren();
	for (auto& it : children)
	{
		if (it->IsDirectory())
		{
			ShowFilesByFilter(it, typefilter, namefilter, callback);
		}
		else
		{
			const String& filename = it->GetName();
			if (MFileUtility::Contain(typefilter, MFileUtility::getExtension(filename)) && filename.find(namefilter) != String::npos)
			{
				String outpath;
				MFileUtility::getFileNameWithPosfix(filename, outpath);
				if (ImGui::Selectable(outpath.c_str(), false, ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0, 0)))
				{
					if (ImGui::IsMouseDoubleClicked(0))
					{
						if (callback.IsBound())
						{
							String dstpath = MFileUtility::GetRelativePath(filename, {});
							callback.Execute(dstpath, nullptr);
						}
						Close();
					}
				}
			}
		}
	}
}

void MSelectWindow::ShowFolderFilesByFilter(Vector<MFileTree*>& filetrees, const Vector<String>& typefilter, const String& namefilter, SelectDelegate& callback)
{
	_AddExtrafileList(filetrees);

	if (m_namefilter.empty())
		showdirectory(filetrees, callback);
	else
		showSerchFoders(filetrees, callback);
}

void MSelectWindow::_AddExtrafileList(Vector<MFileTree*>& filetree)
{
	// 用来判断只在open的时候判断一次
	if (!m_hasInit)
	{
		for (int i = 0; i < m_extraPath.size(); ++i)
		{
			m_filetree.push_back(MFileUtility::GetDirectoryInFileTree(m_extraPath[i], m_typefilter));
		}

		m_hasInit = true;
	}
}

void MSelectWindow::showdirectory(Vector<MFileTree*>& filetrees, SelectDelegate& callback, const String& foldername)
{
	bool showOneFolder = foldername.empty();
	for (int i = 0; i < filetrees.size(); ++i)
	{
		MFileTree* tree = filetrees[i];
		_DoShowdirectory(tree->GetRootNode(), callback, foldername);
	}
}

void MSelectWindow::_DoShowdirectory(MFileItem* fileitem, SelectDelegate& callback, const String& foldername)
{
	String filename = fileitem->GetName();
	ImGui::PushID(filename.c_str());
	ImGui::AlignTextToFramePadding();
	bool is_selected = false;  //s 标识选中状态
	String* selected = EditorSystem::Instance()->GetSelectedString();
	if (selected && *selected == filename)
		is_selected = true;

	String nodeName;
	MFileUtility::getFileNameWithPosfix(filename, nodeName);
	int nodeFlags = MDefined::TreeNodeFlag + ImGuiTreeNodeFlags_NoAutoOpenOnLog;
	if (is_selected)
		nodeFlags = nodeFlags + ImGuiTreeNodeFlags_Selected;

	bool no_children = fileitem->GetChildren().empty();
	if (no_children)
		nodeFlags = nodeFlags + ImGuiTreeNodeFlags_Leaf;

	ImGui::AlignTextToFramePadding();
	bool opened = ImGui::TreeNodeEx(nodeName.c_str(), nodeFlags);

	if (ImGui::IsItemHovered() && ImGui::IsMouseDragging() == false)
	{
		if (ImGui::IsMouseDoubleClicked(0))
		{
			if (MFileUtility::Contain(m_typefilter, MFileUtility::getExtension(filename)))
			{
				String ext = MFileUtility::getExtension(filename);
				String dstpath = MFileUtility::GetRelativePath(filename, m_extraPath);
				if (callback.IsBound())
				{
					callback.Execute(dstpath, CreateEntity(dstpath, ext));
				}
				if (m_warningText.empty())
				{
					if (m_closeCallBack.IsBound())
					{
						m_closeCallBack.Execute(filename);
					}
					Close();
				}
			}
		}
	}

	if (opened)
	{
		if (!no_children)
		{
			const auto& children = fileitem->GetChildren();
			for (auto& it : children)
			{
				_DoShowdirectory(it, callback, foldername);
			}
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void MSelectWindow::showSerchFoders(Vector<MFileTree*>& filetrees, SelectDelegate& callback)
{

}

Object* MSelectWindow::CreateEntity(const String& targetpath, const String& ext)
{
	if (m_type_rtti == nullptr)
		return nullptr;
	
	if (m_type_rtti == &TextureEntity::RTTI())
	{
		String metaPath = MFileUtility::GetTextureMeta(targetpath);
		String rMetaPath;
		MFileUtility::ConverToRelativePath(metaPath, MDefined::ProjectPath, rMetaPath);
		TextureEntity* entity = _NEW TextureEntity();
		entity->PushMetadata(TextureDescribeFileMetadata(rMetaPath));
		entity->CreateResource();

		return entity;
	}
	return nullptr;
}

NS_JYE_END