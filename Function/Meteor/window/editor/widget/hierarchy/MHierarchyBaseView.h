
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"

NS_JYE_BEGIN

class GObject;
class Scene;

class MHierarchyBaseView
{
	DECLARE_DELEGATE_RET(DeleteNodeDelegate, void, GObject*);	
public:
	enum TMHierarchyView
	{
		TMHierarchyView_Scene = 0,
		TMHierarchyView_Layer,
		TMHierarchyView_Reorder,
		TMHierarchyView_Count,
	};
	enum TMHierarchyAction
	{
		TMHierarchyAction_None = 0,
		TMHierarchyAction_Rename,
		TMHierarchyAction_Clone,
		TMHierarchyAction_Count,
	};
	MHierarchyBaseView();
	~MHierarchyBaseView();

	virtual void ClearScene() { m_scene = nullptr; }
	virtual bool OnGui(float dt) = 0;
	virtual void DeletePrompt(GObject* node, DeleteNodeDelegate& deletenodecall);
	void ShowDeletePrompt();

protected:
	virtual TMHierarchyAction _CheckContexMenu(GObject* node);

	bool IsAllowRename(GObject* node);
	bool IsAllowDelete(GObject* node);

	bool m_bShowDeletePromptDialog;
	uint64 m_currentID;
	Scene* m_scene;
	GObject* m_deleteNode;
	DeleteNodeDelegate m_deleteNodeCall;
};

NS_JYE_END