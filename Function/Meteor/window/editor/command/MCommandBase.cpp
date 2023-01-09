#include "MCommandBase.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "Engine/component/Component.h"
#include "../system/EditorSystem.h"

NS_JYE_BEGIN

MCommandBase::MCommandBase()
	: m_IsExcuting(false)
	, m_IsExcutOnlyInGame(false)
	, m_IsSyncCommand(false)
	, m_sceneID(0)
{

}

MCommandBase::~MCommandBase()
{

}

Scene* MCommandBase::GetScene()
{
	return SceneManager::Instance()->GetScenebyId(m_sceneID);
}

GObject* MCommandBase::GetGameObject(uint64 id)
{
	Scene* sce = GetScene();
	GObject* go = nullptr;
	if (sce)
	{
		go = sce->GetGObjectByID(id);
	}
	return go;
}

Component* MCommandBase::GetComponent(uint64 id)
{
	Scene* sce = GetScene();
	GObject* go = nullptr;
	Component* com = nullptr;
	if (sce)
	{
		go = sce->GetGObjectByID(id);
		if (go)
		{
			for (Component* it : go->GetComponents())
			{
				if (it->GetStaticID() == id)
				{
					com = it;
					break;
				}
			}
		}
	}
	return com;
}

Object* MCommandBase::DoIt()
{
	BeginCmd();
	Object* res = _DoIt();
	EndCmd();
	return res;
}

void MCommandBase::Undo()
{
	BeginCmd();
	_Undo();
	EndCmd();
}

void MCommandBase::Redo()
{
	BeginCmd();
	_DoIt();
	EndCmd();
}

void MCommandBase::_CheckEditorSelectNode(uint64 gid)
{
	Scene* scene = GetScene();
	if (scene && scene->GetGObjectByID(gid) == EditorSystem::Instance()->GetSelectedGObject())
	{
		EditorSystem::Instance()->Select((GObject*)nullptr);
	}
}

NS_JYE_END