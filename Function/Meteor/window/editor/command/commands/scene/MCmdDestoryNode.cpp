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
	// TODO : do��ɾ��node������Ҫ�ָ���˼·��ɾ����ʱ�����л���tmpĿ¼��Ȼ��undoȥ�����л����Ƚ��鷳���Ȳ�����
}

NS_JYE_END