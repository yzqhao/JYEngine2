#include "MHierarchyScene.h"
#include "imgui.h"
#include "Engine/component/render/RenderComponent.h"
#include "util/MSceneManagerExt.h"
#include "Engine/object/GObject.h"
#include "Engine/object/Scene.h"
#include "../../system/MDefined.h"
#include "MHierarchyResManager.h"
#include "util/MSceneExt.h"
#include "MHierarchyTool.h"
#include "Math/3DMath.h"
#include "../../system/EditorSystem.h"
#include "window/editor/command/commands/MCmdPropertyChange.h"
#include "window/editor/command/MCommandManager.h"
#include "../../command/commands/node/MCmdCloneNode.h"
#include "imgui_internal.h"
#include "../../command/commands/node/MCmdAttachNode.h"
#include "../../command/commands/node/MCmdDeserializePrefab.h"
#include "util/MFileUtility.h"

NS_JYE_BEGIN

MHierarchyScene::MHierarchyScene()
	: m_bbodyfail(false)
	, m_bedit_lable(false)
{

}

MHierarchyScene::~MHierarchyScene()
{

}

void MHierarchyScene::DrawNode(GObject* node)
{
	if (node == nullptr)
	{
		JYERROR("node is nil");
		return;
	}

	ImGui::PushID((node->GetObjectID()));
	ImGui::AlignTextToFramePadding();
	bool is_selected = false;  // 标识选中状态
	GObject* selected = EditorSystem::Instance()->GetSelectedGObject();
	if (selected && selected->GetObjectID() == node->GetObjectID())
		is_selected = true;

	const String& nodeName = node->GetName();
	uint nodeFlags = MDefined::TreeNodeFlag;

	if (is_selected)
		nodeFlags = nodeFlags | ImGuiTreeNodeFlags_Selected;

	bool no_children = true;
	Vector<GObject*> childList = {};
	MSceneExt::GetNoClothDebugChildrens(node, childList);
	int childCnt = childList.size();
	if (childCnt > 0)
	{
		Vector<GObject*> childs = {};
		for (auto& it : childList)
		{
			if (MHierarchyTool::IsNodeVisible(it))
			{
				if (it->GetEditorUIType() !=  Animatable::UNVISUAL || EditorSystem::Instance()->ShowEverything())
				{
					childs.push_back(it);
				}
				no_children = false;
			}
		}
		std::sort(childs.begin(), childs.end(), [](GObject* a, GObject* b) {
			return a->GetName() < b->GetName();
			});
		childList = childs;
		childCnt = childs.size();
	}

	if (no_children) // 没有子Node, 则创建叶子节点
		nodeFlags = nodeFlags | ImGuiTreeNodeFlags_Leaf;

	bool subSelected = false;
	while (selected)
	{
		if (selected->GetStaticID() == node->GetStaticID())
		{
			subSelected = true;
			break;
		}
		selected = selected->GetRoot();
	}

	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImGui::AlignTextToFramePadding();

	bool isactive = node->isActiveHierarchy();
	ImVec2 size(20, 20);
	ImVec4 color;
	// 当前选中节点是否属于prefab实例
	bool isPrefabRoot = false;
	bool isPrefab = false;
	if (isPrefabRoot)
		color = ImVec4(1, 1, 0, 1);
	else if (isPrefab)
		color = ImVec4(0.7, 0.7, 0, 1);
	else
		color = ImVec4(1, 1, 1, 1);
	color = isactive ? color : ImVec4(0.5, 0.5, 0.5, 1);
	ImGui::PushStyleColor(ImGuiCol_Text, color);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(16, 0));
	bool currActive = node->isActive();
	bool checked = true;
	bool opened = ImGui::TreeNodeEx(nodeName.c_str(), nodeFlags, MHierarchyResManager::Instance()->GetNodeIcon(node), size, checked, currActive, subSelected, &checked);
	if (currActive != checked)
	{
		uint64 StaticID = node->GetStaticID();
		M_CMD_EXEC_BEGIN;
		MCommandManager::Instance()->Execute<MCmdPropertyChangebool>(StaticID, "m_isActive", checked);
		M_CMD_EXEC_END;
	}

	ImGui::PopStyleVar(1);
	ImGui::PopStyleColor(1);

	// 处理拖拽的逻辑，先不做
	if (m_bedit_lable == false)
		CheckDrag(node);  // 处理拖拽
		

	if (m_bedit_lable && is_selected)
	{
		ImGui::SetCursorScreenPos(pos);
		ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
		float contentRegionAvailWidth = contentRegionAvail.x;
		ImGui::PushItemWidth(contentRegionAvailWidth);

		ImGui::PushID(std::to_string(node->GetObjectID()).c_str());

		uint inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.246, 0.246, 0.246, 1.0));
		static char bufnodename[64] = {}; 
		strcpy(bufnodename, nodeName.c_str());
		bool bR = ImGui::InputText("##MHierarchyScene_rename", bufnodename, 64, inputTextFlags);
		ImGui::PopStyleColor(2);
		if (bR)
		{
			uint64 StaticID = node->GetStaticID();
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdPropertyChangeString>(StaticID, "m_name", String(bufnodename));
			M_CMD_EXEC_END;
		}

		ImGui::PopItemWidth();

		//鼠标点击了别处，取消编辑状态
		if (ImGui::IsItemActive() == false && (ImGui::IsMouseClicked(0) || ImGui::IsMouseDragging()))
		{
			// todo
			m_bedit_lable = false;
		}
		ImGui::PopID();
	}

	if (ImGui::IsItemHovered() && ImGui::IsMouseDragging() == false)
	{
		if (ImGui::IsMouseClicked(0))
			m_currentID = ImGui::GetID(std::to_string(node->GetObjectID()).c_str());

		uint localID = ImGui::GetID(std::to_string(node->GetObjectID()).c_str());

		if (ImGui::IsMouseReleased(0) && m_currentID == localID)
		{
			if (is_selected == false)
				m_bedit_lable = false;
			EditorSystem::Instance()->Select(node); //选中此node
		}

		// 双击场景中选中此node
		if (ImGui::IsMouseDoubleClicked(0))
		{
			// TODO
		}
	}

	if (m_bedit_lable == false)
	{
		auto action = _CheckContexMenu(node);  //右键菜单
		if (action == TMHierarchyAction_Rename)
		{
			m_bedit_lable = true;
			EditorSystem::Instance()->Select(node);
		}
		else if (action == TMHierarchyAction_Clone)
		{
			uint64 StaticID = node->GetStaticID();
			M_CMD_EXEC_BEGIN;
			MCommandManager::Instance()->Execute<MCmdCloneNode>(StaticID);
			M_CMD_EXEC_END;
		}
	}

	if (opened)
	{
		if (!no_children) //迭代子节点
			for (int i = 0; i < childCnt; ++i)
				DrawNode(childList[i]);
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void MHierarchyScene::Render(GObject* node)
{
	if (m_bbodyfail)
	{
		uint windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDocking;
		bool bshow = true;
		bool b1 = ImGui::Begin("Body Fail", &bshow, windowFlags);
		ImGui::Text("scene not select main camera!");
		if (ImGui::Button("Ok "))
			m_bbodyfail = false;
		ImGui::End();
	}

	_CheckContexMenu(node);
	DrawNode(node);
	ShowDeletePrompt();
}

bool MHierarchyScene::OnGui(float dt)
{
	if (m_scene == nullptr)
	{
		m_scene = MSceneManagerExt::Instance()->GetEditScene();
		if (m_scene == nullptr)
		{
			return false;
		}
	}

	GObject* rootnode = m_scene->GetRootNode();

	ImGui::BeginChild("background"); // 处理拖拽到空白处的逻辑
	uint flag = MDefined::TreeNodeFlag + ImGuiTreeNodeFlags_DefaultOpen;
	ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 16);
	ImGui::AlignTextToFramePadding();
	bool btemselect = false;
	bool selected = ImGui::TreeNodeEx(m_scene->GetName().c_str(), flag, MHierarchyResManager::Instance()->GetNodeIcon(rootnode), 
		ImVec2(20, 20), false, false, false, &btemselect);
	if (selected)
	{
		if (ImGui::IsItemClicked())
		{
			EditorSystem::Instance()->Select(m_scene);
		}
		Render(rootnode);
		ImGui::TreePop();
	}
	ImGui::PopStyleVar();
	ImGui::EndChild();

	return true;
}

void MHierarchyScene::CheckDrag(GObject* node)
{
	if (_ProcessDragDropSource(node) == false)
		_ProcessDragDropTarget(node);
}

bool MHierarchyScene::_ProcessDragDropSource(GObject* node)
{
	// 根节点 root不能拖动
	if (node && ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		const String& nodeName = node->GetName();
		ImGui::TextUnformatted(nodeName.c_str());  //拖拽时显示node名称
		uint64 id = node->GetStaticID();
		String strid = std::to_string(id);
		ImGui::SetDragDropPayload("Node", strid.c_str(), strid.size()); //将ID传递过去，之后再通过ID查找NODE
		ImGui::EndDragDropSource();
		return true;
	}
	return false;
}

void MHierarchyScene::_ProcessDragDropTarget(GObject* node)
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
			if (dropNode && dropNode->GetStaticID() != m_scene->GetRootNode()->GetStaticID())	// root node不允许拖动
			{
				M_CMD_EXEC_BEGIN;
				MCommandManager::Instance()->Execute<MCmdAttachNode>(node->GetStaticID(), dropNode->GetStaticID());
				M_CMD_EXEC_END;
			}
		}
		const ImGuiPayload* payloadAtapterAssets = ImGui::AcceptDragDropPayload("assets");
		if (payloadAtapterAssets)
		{
			String strpath((char*)payloadAtapterAssets->Data, payloadAtapterAssets->DataSize);
			M_CMD_EXEC_BEGIN;
			GObject* prefabNode = MCommandManager::Instance()->Execute<MCmdDeserializePrefab>(node->GetStaticID(), strpath);
			M_CMD_EXEC_END;
			EditorSystem::Instance()->Select(prefabNode);
		}
		ImGui::EndDragDropTarget();
	}
}

NS_JYE_END