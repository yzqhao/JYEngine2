#include "MCmdDestoryNode.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "util/MSceneExt.h"
#include "Engine/component/transform/TransformComponent.h"
#include "Engine/object/GObject.h"
#include "../../../system/EditorSystem.h"

NS_JYE_BEGIN

MCmdDestoryNode::MCmdDestoryNode(uint64 gid)
	: m_nodeID(gid)
{

}

MCmdDestoryNode::~MCmdDestoryNode()
{

}

Object* MCmdDestoryNode::_DoIt()
{
	Scene* scene = GetScene();
	if (scene)
	{
		_CheckEditorSelectNode(m_nodeID);
		scene->DeleteGObjectByID(m_nodeID);
	}
	return nullptr;
}

void MCmdDestoryNode::_Undo()
{
	// TODO : do是删除node，这里要恢复。思路是删除的时候序列化到tmp目录，然后undo去反序列化。比较麻烦，先不处理
}

NS_JYE_END