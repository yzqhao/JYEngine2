
#include "CameraGizmo.h"
#include "CameraComponent.h"
#include "Engine/object/GObject.h"
#include "Engine/object/Scene.h"
#include "Engine/object/GObject.h"
#include "Engine/object/Scene.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/component/transform/TransformComponent.h"
#include "Engine/resource/TextureMetadata.h"
#include "Engine/component/GizmoConfig.h"
#include "Engine/resource/VertexMetadata.h"
#include "Engine/resource/IndicesMetadata.h"
#include "Engine/render/texture/TextureEntity.h"
#include "Engine/render/material/IMaterialSystem.h"
#include "Engine/render/material/MaterialEntity.h"
#include "Engine/resource/GeneralMetadata.h"

NS_JYE_BEGIN

CameraGizmo::CameraGizmo()
{
	m_pFrustumNode = NULL;
	m_pGizmoNode = NULL;
	m_isCreateFromNode = false;
}

CameraGizmo::~CameraGizmo()
{
	
}

void CameraGizmo::CreateSource(const Math::Frustum& frustum, const CameraComponent* camera) //创建图标和视锥体
{
	if (m_isCreateFromNode)
	{
		return;
	}
	GObject* host = (GObject*)camera->GetParentObject();
	Scene* scene = host->GetScene();
	m_isCreateFromNode = true;
	m_pFrustumNode = scene->CreateObject("frustum_node");
	m_pGizmoNode = scene->CreateObject("gizmo_node");
	host->AddChild(m_pFrustumNode);
	host->AddChild(m_pGizmoNode);
	m_pFrustumNode->SetLayer(MC_MASK_EDITOR_SCENE_LAYER);
	m_pGizmoNode->SetLayer(MC_MASK_EDITOR_SCENE_LAYER);

	//视锥体绘制
	RenderComponent* linerc = m_pFrustumNode->CreateComponent<RenderComponent>();
	linerc->EraseRenderProperty(GraphicDefine::RP_CULL);
	linerc->SetBindBox(Math::AABB(Math::Vec3(0, 0, 0), Math::Vec3(0, 0, 0)));

	//std::vector<Math::Vec3> frustumpointset;
	m_FrustumVertex.SetVertexType(RHIDefine::PS_ATTRIBUTE_POSITION,
		RHIDefine::DT_FLOAT,
		RHIDefine::DT_HALF_FLOAT,
		3);
	m_FrustumVertex.ReserveBuffer(8);
	for (int i = 0; i < 8; i++)
	{
		m_FrustumVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_POSITION, frustum.GetCorner(static_cast<Math::Frustum::CornerName>(i)).GetPtr());
	}

	m_FrustumIndices.SetIndicesType(RHIDefine::IT_UINT16);
	m_FrustumIndices.ReserveBuffer(24);
	int IndexTable[24] = { 2,6,3,7,0,4,1,5,0,1,1,3,3,2,2,0,4,5,5,7,7,6,6,4 };
	for (int i = 0; i < 24; i++)
	{
		m_FrustumIndices.PushIndicesData(IndexTable[i]);
	}

	MaterialEntity* mat = _NEW MaterialEntity();
	mat->PushMetadata(PathMetadata(GizmoConfig::GIZMO_MATERIAL_PATH));
	mat->CreateResource();
	linerc->AddMaterialEntity(mat);
	RHIDefine::ParameterSlot gridColor = IMaterialSystem::Instance()->NewParameterSlot(RHIDefine::SU_UNIFORM, "POINT_COLOR");
	linerc->PushMetadata(RenderObjectMeshMetadate(RHIDefine::RM_LINES, 
		ReferenceVertexMetadata(RHIDefine::MU_DYNAMIC, &m_FrustumVertex), 
		ReferenceIndicesMetadata(RHIDefine::MU_STATIC, &m_FrustumIndices)));
	linerc->CreateResource();
	Math::Vec4 color(0.5, 0.5, 0.5, 1.0);
	linerc->SetParameter(gridColor, MakeMaterialParam(color));

	//贴图资源
	m_pGizmoNode->SetName(GizmoConfig::GIZMO_CAMERA);//方便拾取做特殊处理
	m_pGizmoNode->CreateComponent<TransformComponent>();//贴图shader用到了transform
	RenderComponent* quadrc = m_pGizmoNode->CreateComponent<RenderComponent>();
	quadrc->SetBindBox(Math::AABB(Math::Vec3(-0.05, -0.05, -0.05), Math::Vec3(0.05, 0.05, 0.05)));
	mat = _NEW MaterialEntity();
	mat->PushMetadata(PathMetadata(GizmoConfig::QUADTEX_MATERIAL_PATH));
	mat->CreateResource();
	TextureEntity* tex = _NEW TextureEntity();
	tex->PushMetadata(TextureFileMetadata(
		RHIDefine::TextureType::TEXTURE_2D,
		RHIDefine::TU_STATIC,
		RHIDefine::PF_AUTO, 1, true, 0,
		RHIDefine::TW_CLAMP_TO_EDGE,
		RHIDefine::TW_CLAMP_TO_EDGE,
		RHIDefine::TF_LINEAR,
		RHIDefine::TF_LINEAR_MIPMAP_LINEAR,
		GizmoConfig::CAMERA_TEX_PATH));
	tex->CreateResource();
	mat->SetParameter(RHIDefine::PS_TEXTURE_DIFFUSE, MakeMaterialParam(tex));
	quadrc->AddMaterialEntity(mat);

	m_QuadVertex.SetVertexType(RHIDefine::PS_ATTRIBUTE_POSITION,
		RHIDefine::DT_FLOAT,
		RHIDefine::DT_HALF_FLOAT,
		4);

	m_QuadVertex.SetVertexType(RHIDefine::PS_ATTRIBUTE_COORDNATE1,
		RHIDefine::DT_FLOAT,
		RHIDefine::DT_HALF_FLOAT,
		2);
	m_QuadVertex.SetVertexType(RHIDefine::PS_ATTRIBUTE_COORDNATE0,
		RHIDefine::DT_FLOAT,
		RHIDefine::DT_HALF_FLOAT,
		2);
	m_QuadVertex.ReserveBuffer(4);
	for (int i = 0; i < 4; i++)
	{
		m_QuadVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(0, 0, 0, 1).GetPtr());
	}
	m_QuadVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_COORDNATE1, Math::Vec2(-0.1, 0.1).GetPtr());
	m_QuadVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_COORDNATE1, Math::Vec2(-0.1, -0.1).GetPtr());
	m_QuadVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_COORDNATE1, Math::Vec2(0.1, 0.1).GetPtr());
	m_QuadVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_COORDNATE1, Math::Vec2(0.1, -0.1).GetPtr());

	m_QuadVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(0, 0).GetPtr());
	m_QuadVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(0, 1).GetPtr());
	m_QuadVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(1, 0).GetPtr());
	m_QuadVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(1, 1).GetPtr());


	m_QuadIndices.SetIndicesType(RHIDefine::IT_UINT16);
	m_QuadIndices.ReserveBuffer(6);
	m_QuadIndices.PushIndicesData(0);
	m_QuadIndices.PushIndicesData(1);
	m_QuadIndices.PushIndicesData(2);
	m_QuadIndices.PushIndicesData(1);
	m_QuadIndices.PushIndicesData(2);
	m_QuadIndices.PushIndicesData(3);

	quadrc->PushMetadata(RenderObjectMeshMetadate(RHIDefine::RM_TRIANGLES, 
		ReferenceVertexMetadata(RHIDefine::MU_STATIC, &m_QuadVertex), 
		ReferenceIndicesMetadata(RHIDefine::MU_STATIC, &m_QuadIndices)));
	quadrc->CreateResource();

	//无法区分摄像机 所以全都画辅助体 然后设置成不可见
	quadrc->EraseRenderProperty(GraphicDefine::RP_SHOW);
	linerc->EraseRenderProperty(GraphicDefine::RP_SHOW);
#ifdef _EDITOR
	quadrc->SetRenderProperty(GraphicDefine::RP_SHOW);  //编辑器模式下相机图片全部可见
#endif

}

void CameraGizmo::UpdateSource(const Math::Frustum& frustum) //更新视锥体
{
	if (!m_isCreateFromNode)
	{
		return;
	}

	RenderComponent* rc = m_pFrustumNode->TryGetComponent<RenderComponent>();
	uint index_vertex = m_FrustumVertex.GetAttributeIndex(RHIDefine::PS_ATTRIBUTE_POSITION);
	for (int i = 0; i < 8; i++)
	{
		m_FrustumVertex.ChangeVertexDataWithAttributeFast(index_vertex, i, frustum.GetCorner(static_cast<Math::Frustum::CornerName>(i)).GetPtr());
	}
	m_FrustumVertex.SetReflushInterval(0, 7);
	rc->ChangeVertexBuffer(m_FrustumVertex);
}

void CameraGizmo::SetFrustumShow(bool show)
{
	RenderComponent* rc = m_pFrustumNode->TryGetComponent<RenderComponent>();
	if (show)
	{
		rc->SetRenderProperty(GraphicDefine::RP_SHOW);
	}
	else
	{
		rc->EraseRenderProperty(GraphicDefine::RP_SHOW);
	}
}

void CameraGizmo::SetGizmoShow(bool show)
{
	RenderComponent* rc = m_pGizmoNode->TryGetComponent<RenderComponent>();
	if (show)
	{
		rc->SetRenderProperty(GraphicDefine::RP_SHOW);
	}
	else
	{
		rc->EraseRenderProperty(GraphicDefine::RP_SHOW);
	}
}

bool CameraGizmo::GetGizmoActive()
{
	return m_pGizmoNode->isActive();
}

void CameraGizmo::SetGizmoActive(bool isActive)
{
	m_pGizmoNode->SetActive(isActive);
}

NS_JYE_END