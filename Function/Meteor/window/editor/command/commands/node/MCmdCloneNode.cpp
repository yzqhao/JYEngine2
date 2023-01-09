#include "MCmdCloneNode.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"

NS_JYE_BEGIN

MCmdCloneNode::MCmdCloneNode(int64 id)
	: m_goID(id)
	, m_cloneGoID(0)
{

}

MCmdCloneNode::~MCmdCloneNode()
{

}

Object* MCmdCloneNode::_DoIt()
{
	Scene* scene = GetScene();
	GObject* node = GetGameObject(m_goID);
	GObject* clonenode = nullptr;
	if (node && scene)
	{
		clonenode = static_cast<GObject*>(node->CloneNode());
		m_cloneGoID = clonenode->GetStaticID();
	}
	return clonenode;
}

void MCmdCloneNode::_Undo()
{
	Scene* scene = GetScene();
	if (scene)
	{
		scene->DeleteGObjectByID(m_cloneGoID);
	}
}

NS_JYE_END