#include "MCmdRemoveMaterial.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/resource/MaterialMetadata.h"
#include "Engine/resource/MaterialStreamData.h"

NS_JYE_BEGIN

MCmdRemoveMaterial::MCmdRemoveMaterial(int64 id, int idx)
	: m_goID(id)
	, m_idx(idx)
{

}

MCmdRemoveMaterial::~MCmdRemoveMaterial()
{

}

Object* MCmdRemoveMaterial::_DoIt()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		MaterialEntity* mat = ren->GetMaterialEntity(m_idx-1);
		m_matpath = mat->GetMatPath();
		ren->ResizeMaterials(m_idx - 1);
	}
	return component;
}

void MCmdRemoveMaterial::_Undo()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		MaterialEntity* mat = _NEW MaterialEntity();
		mat->PushMetadata(MaterialMetadata(m_matpath));
		mat->CreateResource();
		ren->ChangeMaterialEntity(m_idx-1, mat);
	}
}

NS_JYE_END