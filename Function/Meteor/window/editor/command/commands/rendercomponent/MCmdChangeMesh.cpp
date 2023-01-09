#include "MCmdChangeMesh.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/render/RenderComponent.h"
#include "util/MFileUtility.h"

NS_JYE_BEGIN

MCmdChangeMesh::MCmdChangeMesh(int64 id, const RenderObjectMeshFileMetadate& meta)
	: m_goID(id)
	, m_comID(0)
	, m_meta(meta)
{

}

MCmdChangeMesh::~MCmdChangeMesh()
{

}

Object* MCmdChangeMesh::_DoIt()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		String meshname = ren->GetRenderObjectEntity()->GetMeshName();
		if (MFileUtility::getExtension(meshname) == "mesh")
		{
			m_premeta = RenderObjectMeshFileMetadate(RHIDefine::MU_STATIC, meshname);
		}
		else if (MFileUtility::getExtension(meshname) == "dynamicmesh")
		{
			m_premeta = RenderObjectMeshFileMetadate(RHIDefine::MU_DYNAMIC, meshname);
		}
		ren->ChangeRenderObjectEntity(m_meta);
	}
	return component;
}

void MCmdChangeMesh::_Undo()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		ren->ChangeRenderObjectEntity(m_premeta);
	}
}

NS_JYE_END