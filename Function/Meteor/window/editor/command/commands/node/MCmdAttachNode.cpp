#include "MCmdAttachNode.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "util/MSceneExt.h"

NS_JYE_BEGIN

MCmdAttachNode::MCmdAttachNode(uint64 id, uint64 childid)
	: m_goID(id)
	, m_childID(childid)
	, m_preID(0)
{

}

MCmdAttachNode::~MCmdAttachNode()
{

}

Object* MCmdAttachNode::_DoIt()
{
	Scene* scene = GetScene();
	GObject* rootnode = GetGameObject(m_goID);
	GObject* child = GetGameObject(m_childID);
	if (rootnode && child && rootnode != child && rootnode != child->GetRoot())
	{
		m_preID = child->GetRoot()->GetStaticID();
		rootnode->AddChild(child);
	}
	return rootnode;
}

void MCmdAttachNode::_Undo()
{
	Scene* scene = GetScene();
	GObject* rootnode = GetGameObject(m_goID);
	GObject* child = GetGameObject(m_childID);
	if (m_preID!=0)
	{
		GObject* preRoot = GetGameObject(m_preID);
		preRoot->AddChild(child);
	}
}

NS_JYE_END