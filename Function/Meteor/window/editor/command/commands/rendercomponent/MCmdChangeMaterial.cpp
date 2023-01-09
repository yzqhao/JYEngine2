#include "MCmdChangeMaterial.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/resource/MaterialMetadata.h"

NS_JYE_BEGIN

MCmdChangeMaterial::MCmdChangeMaterial(int64 id, const String& matname, int matIdx)
	: m_goID(id)
	, m_matIdx(0)
	, m_matname(matname)
{

}

MCmdChangeMaterial::~MCmdChangeMaterial()
{

}

Object* MCmdChangeMaterial::_DoIt()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		MaterialEntity* mat = ren->GetMaterialEntity(m_matIdx);
		m_pre_matname = mat->GetMatPath();
		
		MaterialEntity* nmat = _NEW MaterialEntity();
		nmat->PushMetadata(MaterialMetadata(m_matname));
		nmat->CreateResource();
		ren->ChangeMaterialEntity(m_matIdx, nmat);
	}
	return component;
}

void MCmdChangeMaterial::_Undo()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		MaterialEntity* nmat = _NEW MaterialEntity();
		nmat->PushMetadata(MaterialMetadata(m_pre_matname));
		nmat->CreateResource();
		ren->ChangeMaterialEntity(m_matIdx, nmat);
	}
}

NS_JYE_END