#include "MCmdAddMaterial.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/resource/MaterialMetadata.h"

NS_JYE_BEGIN

MCmdAddMaterial::MCmdAddMaterial(int64 id, const String& matpath, int idx)
	: m_goID(id)
	, m_matpath(matpath)
	, m_idx(idx)
{

}

MCmdAddMaterial::~MCmdAddMaterial()
{

}

Object* MCmdAddMaterial::_DoIt()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		MaterialEntity* mat = _NEW MaterialEntity();
		mat->PushMetadata(MaterialMetadata(m_matpath));
		mat->CreateResource();
		ren->ChangeMaterialEntity(m_idx, mat);
	}
	return component;
}

void MCmdAddMaterial::_Undo()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		ren->ResizeMaterials(m_idx);
	}
}

NS_JYE_END