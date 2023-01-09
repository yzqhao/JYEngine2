#include "MCmdCreateGenericNode.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "util/MSceneExt.h"
#include "Engine/component/transform/TransformComponent.h"
#include "Engine/object/GObject.h"
#include "../../../system/EditorSystem.h"

NS_JYE_BEGIN

MCmdCreateGenericNode::MCmdCreateGenericNode(const String& goname)
	: m_goName(goname)
	, m_nodeID(0)
	, m_transID(0)
{

}

MCmdCreateGenericNode::~MCmdCreateGenericNode()
{

}

Object* MCmdCreateGenericNode::_DoIt()
{
	GObject* node = nullptr;
	Scene* scene = GetScene();
	if (scene)
	{
		node = MSceneExt::SceneCreateGenericNode(scene, m_goName);
		// GenericNode里边会默认创建TransComponent
		// 我们要保证TansformComponent的ID和Peview场景要一致
		TransformComponent* trans = node->TryGetComponent<TransformComponent>();
		if (m_transID == 0)
		{
			m_nodeID = node->GetStaticID();
			m_transID = trans->GetStaticID();
		}
		else
		{
			node->SetStaticID(m_nodeID);
			trans->SetStaticID(m_transID);
		}
	}
	return node;
}

void MCmdCreateGenericNode::_Undo()
{
	Scene* scene = GetScene();
	if (scene)
	{
		_CheckEditorSelectNode(m_nodeID);
		scene->DeleteGObjectByID(m_nodeID);
	}
}

NS_JYE_END