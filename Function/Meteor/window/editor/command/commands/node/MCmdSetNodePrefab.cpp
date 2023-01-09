#include "MCmdSetNodePrefab.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "util/MSceneExt.h"

NS_JYE_BEGIN

MCmdSetNodePrefab::MCmdSetNodePrefab(uint64 id, const String& prefabPath)
	: m_goID(id)
	, m_prefabPath(prefabPath)
{

}

MCmdSetNodePrefab::~MCmdSetNodePrefab()
{

}

Object* MCmdSetNodePrefab::_DoIt()
{
	Scene* scene = GetScene();
	GObject* node = GetGameObject(m_goID);
	if (node && scene)
	{
		MSceneExt::SetPrefabPathInEdSc(node, m_prefabPath);
	}
	return node;
}

void MCmdSetNodePrefab::_Undo()
{
	Scene* scene = GetScene();
	GObject* node = GetGameObject(m_goID);
	if (scene)
	{
		MSceneExt::SetPrefabPathInEdSc(node, "");
	}
}

NS_JYE_END