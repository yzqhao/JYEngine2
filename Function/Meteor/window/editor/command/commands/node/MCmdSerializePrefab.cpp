#include "MCmdSerializePrefab.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "util/MSceneExt.h"
#include "util/MFileUtility.h"

NS_JYE_BEGIN

MCmdSerializePrefab::MCmdSerializePrefab(uint64 id, const String& savepath)
	: m_goID(id)
	, m_prefabPath(savepath)
{

}

MCmdSerializePrefab::~MCmdSerializePrefab()
{

}

Object* MCmdSerializePrefab::_DoIt()
{
	Scene* scene = GetScene();
	GObject* node = GetGameObject(m_goID);
	if (scene)	
	{
		if (node && scene)
		{
			Stream stream;
			stream.SetStreamFlag(Stream::AT_REGISTER);
			stream.ArchiveAll(node);
			stream.Save(m_prefabPath.c_str());
			MFileUtility::AppendToFileTree(m_prefabPath);
		}
	}
	return node;
}

void MCmdSerializePrefab::_Undo()
{
	Scene* scene = GetScene();
	GObject* node = GetGameObject(m_goID);
	if (node && scene)
	{
		// É¾³ýprefabÎÄ¼þ
		// IFileSystem::Instance()->
		MFileUtility::RemoveInFileTree(m_prefabPath);
	}
}

NS_JYE_END