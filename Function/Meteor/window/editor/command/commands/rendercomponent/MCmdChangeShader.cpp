#include "MCmdChangeShader.h"
#include "Engine/object/SceneManager.h"
#include "Engine/object/Scene.h"
#include "util/MSceneManagerExt.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/resource/MaterialMetadata.h"
#include "../../../widget/assets/MatTool.h"

NS_JYE_BEGIN

MCmdChangeShader::MCmdChangeShader(int64 id, const String& shaderpath, int matIdx)
	: m_goID(id)
	, m_matIdx(0)
	, m_shaderpath(shaderpath)
{

}

MCmdChangeShader::~MCmdChangeShader()
{

}

Object* MCmdChangeShader::_DoIt()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		MaterialEntity* mat = ren->GetMaterialEntity(m_matIdx);
		String matpath = mat->GetMatPath();
		String m_pre_shaderpath = mat->GetShaderPath();
		MatTool::CreateMat(matpath, m_shaderpath);
		
		MaterialEntity* nmat = _NEW MaterialEntity();
		nmat->PushMetadata(MaterialMetadata(matpath));
		nmat->CreateResource();
		ren->ChangeMaterialEntity(m_matIdx, nmat);
	}
	return component;
}

void MCmdChangeShader::_Undo()
{
	Component* component = GetComponent(m_goID);
	RenderComponent* ren = static_cast<RenderComponent*>(component);
	if (ren)
	{
		MaterialEntity* mat = ren->GetMaterialEntity(m_matIdx);
		String matpath = mat->GetMatPath();
		MatTool::CreateMat(matpath, m_pre_shaderpath);
		
		MaterialEntity* nmat = _NEW MaterialEntity();
		nmat->PushMetadata(MaterialMetadata(matpath));
		nmat->CreateResource();
		ren->ChangeMaterialEntity(m_matIdx, nmat);
	}
}

NS_JYE_END