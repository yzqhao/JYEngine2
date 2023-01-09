#include "MCmdDeserializePrefab.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "util/MSceneExt.h"
#include "../../../system/BundleManager.h"

NS_JYE_BEGIN

MCmdDeserializePrefab::MCmdDeserializePrefab(uint64 id, const String& prefabpath)
	: m_goID(id)
	, m_prefabID(0)
	, m_prefabPath(prefabpath)
{

}

MCmdDeserializePrefab::~MCmdDeserializePrefab()
{

}

Object* MCmdDeserializePrefab::_DoIt()
{
	Scene* scene = GetScene();
	GObject* node = GetGameObject(m_goID);
	GObject* obj = nullptr;
	if (node)
	{
		obj = BundleManager::Deserialize<GObject>(m_prefabPath);
		m_prefabID = obj->GetStaticID();
		node->AddChild(obj);
		scene->AddToScene(obj);

		String newname = MSceneExt::GenerateNonRepeatName(obj);
		obj->SetName(newname);
	}
	return obj;
}

void MCmdDeserializePrefab::_Undo()
{
	Scene* scene = GetScene();
	GObject* node = GetGameObject(m_goID);
	GObject* prefabnode = GetGameObject(m_prefabID);
	if (node && prefabnode)
	{
		node->DetachNode(prefabnode);
	}
}

NS_JYE_END