
#include "LightGizmo.h"
#include "LightComponent.h"
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

LightGizmo::LightGizmo()
{
	m_pLightNode = NULL;
	m_pGizmoNode = NULL;
	m_isCreateFromNode = false;
}

LightGizmo::~LightGizmo()
{

}

void LightGizmo::CreateLight(const LightComponent* lc)
{
	if (m_isCreateFromNode)
	{
		return;
	}
	m_isCreateFromNode = true;

	m_pHost = (GObject*)lc->GetParentObject();

	Scene* scene = m_pHost->GetScene();
	m_pLightNode = scene->CreateObject("light_node");
	m_pGizmoNode = scene->CreateObject("gizmo_node");
	m_pHost->AddChild(m_pGizmoNode);
	m_pHost->AddChild(m_pLightNode);
	m_pLightNode->SetLayer(MC_MASK_EDITOR_SCENE_LAYER);
	m_pGizmoNode->SetLayer(MC_MASK_EDITOR_SCENE_LAYER);

	//光线绘制
	RenderComponent* linerc = m_pLightNode->CreateComponent<RenderComponent>();
	linerc->EraseRenderProperty(GraphicDefine::RP_CULL);
	linerc->SetBindBox(Math::AABB(Math::Vec3(0, 0, 0), Math::Vec3(0, 0, 0)));
	m_LightVertex.SetVertexType(RHIDefine::PS_ATTRIBUTE_POSITION, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT, 3);
	m_LightIndices.SetIndicesType(RHIDefine::IT_UINT16);
	m_LightVertex.ReserveBuffer(0);
	m_LightIndices.ReserveBuffer(0);
	m_pointnumpercircle = 0;
	switch (lc->GetLightType())
	{
	case  GraphicDefine::LT_DIRECTIONAL:
	{
		m_eCurrentLightType = GraphicDefine::LT_DIRECTIONAL;
		m_GizmoPath = GizmoConfig::DIRECTION_LIGHT_TEX_PATH;
		CreateDirectionLight(lc);
		break;
	}
	case  GraphicDefine::LT_POINT:
	{
		m_eCurrentLightType = GraphicDefine::LT_POINT;
		m_GizmoPath = GizmoConfig::POINT_LIGHT_TEX_PATH;
		CreatePointLight(lc);
		break;
	}
	case  GraphicDefine::LT_SPOT:
	{
		m_eCurrentLightType = GraphicDefine::LT_SPOT;
		m_GizmoPath = GizmoConfig::SPOT_LIGHT_TEX_PATH;
		CreateSpotLight(lc);
		break;
	}
	default:
	{
		m_eCurrentLightType = GraphicDefine::LT_AMBIENT;
		m_GizmoPath = GizmoConfig::AMBIENT_LIGHT_TEX_PATH;
		break;
	}
	}

	MaterialEntity* mat = _NEW MaterialEntity();
	mat->PushMetadata(PathMetadata(GizmoConfig::GIZMO_MATERIAL_PATH));
	mat->CreateResource();
	linerc->AddMaterialEntity(mat);
	RHIDefine::ParameterSlot gridColor = IMaterialSystem::Instance()->NewParameterSlot(RHIDefine::SU_UNIFORM, "POINT_COLOR");
	linerc->PushMetadata(RenderObjectMeshMetadate(RHIDefine::RM_LINES, 
		ReferenceVertexMetadata(RHIDefine::MU_DYNAMIC, &m_LightVertex), 
		ReferenceIndicesMetadata(RHIDefine::MU_DYNAMIC, &m_LightIndices)));
	linerc->CreateResource();
	Math::Vec4 color(0.5, 0.5, 0.0, 1.0);
	linerc->SetParameter(gridColor, MakeMaterialParam(color));

	// RenderComponent 初始化时， 在CreateResource后，CreateSubMesh()
	linerc->CreateSubMesh();

	//贴图绘制
	m_pGizmoNode->SetName(GizmoConfig::GIZMO_LIGHT);//方便拾取做特殊处理
	m_pGizmoNode->CreateComponent<TransformComponent>();
	RenderComponent* quadrc = m_pGizmoNode->CreateComponent<RenderComponent>();
	//std::cout << "贴图com的地址为" << quadrc << std::endl;
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
		m_GizmoPath));
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
}

void LightGizmo::CreateDirectionLight(const LightComponent* lc)
{
	const Math::Vec3& lightpos = lc->GetWorldPosition();
	const Math::Vec3& lightup = lc->GetWorldUp();
	const Math::Vec3& lightdir = lc->GetWorldDirection();
	const Math::Vec3&& lightright = lightup.Cross(lightdir);
	m_pointnumpercircle = 8;
	std::vector<Math::Vec3>&& tablez = GetCirclePoint(lightpos, lightright, lightup, m_pointnumpercircle, 0.1);
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_POSITION, tablez[i].GetPtr());
	}
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_POSITION, (tablez[i] + lightdir).GetPtr());
	}
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightIndices.PushIndicesData(i);
		if (i == m_pointnumpercircle - 1)
		{
			m_LightIndices.PushIndicesData(0);
		}
		else
		{
			m_LightIndices.PushIndicesData(i + 1);
		}
	}
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightIndices.PushIndicesData(i);
		m_LightIndices.PushIndicesData(i + m_pointnumpercircle);
	}
}

void LightGizmo::CreatePointLight(const LightComponent* lc)
{
	const Math::Vec3& lightpos = lc->GetWorldPosition();
	Math::Vec3 lightup(0, 1, 0);
	Math::Vec3 lightdir(0, 0, 1);
	Math::Vec3 lightright(1, 0, 0);
	m_pointnumpercircle = 20;
	float range = lc->GetRange();
	std::vector<Math::Vec3>&& tablex = GetCirclePoint(lightpos, lightup, lightdir, m_pointnumpercircle, range);
	std::vector<Math::Vec3>&& tabley = GetCirclePoint(lightpos, lightright, lightdir, m_pointnumpercircle, range);
	std::vector<Math::Vec3>&& tablez = GetCirclePoint(lightpos, lightright, lightup, m_pointnumpercircle, range);
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_POSITION, tablex[i].GetPtr());
		m_LightIndices.PushIndicesData(i);
		if (i == m_pointnumpercircle - 1)
		{
			m_LightIndices.PushIndicesData(0);
		}
		else
		{
			m_LightIndices.PushIndicesData(i + 1);
		}
	}
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_POSITION, tabley[i].GetPtr());
		m_LightIndices.PushIndicesData(i + m_pointnumpercircle);
		if (i == m_pointnumpercircle - 1)
		{
			m_LightIndices.PushIndicesData(m_pointnumpercircle);
		}
		else
		{
			m_LightIndices.PushIndicesData(i + 1 + m_pointnumpercircle);
		}
	}
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_POSITION, tablez[i].GetPtr());
		m_LightIndices.PushIndicesData(i + 2 * m_pointnumpercircle);
		if (i == m_pointnumpercircle - 1)
		{
			m_LightIndices.PushIndicesData(2 * m_pointnumpercircle);
		}
		else
		{
			m_LightIndices.PushIndicesData(i + 1 + 2 * m_pointnumpercircle);
		}
	}
}

void LightGizmo::CreateSpotLight(const LightComponent* lc)
{
	const Math::Vec3& lightpos = lc->GetWorldPosition();
	const Math::Vec3& lightup = lc->GetWorldUp();
	const Math::Vec3& lightdir = lc->GetWorldDirection();
	const Math::Vec3&& lightright = lightup.Cross(lightdir);
	m_pointnumpercircle = 8;
	Math::Vec2 range = lc->GetLightAngle();
	float innerangle = range.x / 2.0;
	float outerangle = range.y / 2.0;
	std::vector<Math::Vec3>&& tablein = GetCirclePoint(lightpos + lightdir * lc->GetRange(), lightup * lc->GetRange(), lightright * lc->GetRange(), m_pointnumpercircle, tan(innerangle));
	std::vector<Math::Vec3>&& tableout = GetCirclePoint(lightpos + lightdir * lc->GetRange(), lightup * lc->GetRange(), lightright * lc->GetRange(), m_pointnumpercircle, tan(outerangle));
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_POSITION, tablein[i].GetPtr());
		m_LightIndices.PushIndicesData(i);
		if (i == m_pointnumpercircle - 1)
		{
			m_LightIndices.PushIndicesData(0);
		}
		else
		{
			m_LightIndices.PushIndicesData(i + 1);
		}
	}
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_POSITION, tableout[i].GetPtr());
		m_LightIndices.PushIndicesData(i + m_pointnumpercircle);
		if (i == m_pointnumpercircle - 1)
		{
			m_LightIndices.PushIndicesData(m_pointnumpercircle);
		}
		else
		{
			m_LightIndices.PushIndicesData(i + 1 + m_pointnumpercircle);
		}
	}
	m_LightVertex.PushVertexData(RHIDefine::PS_ATTRIBUTE_POSITION, lightpos.GetPtr());
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightIndices.PushIndicesData(i);
		m_LightIndices.PushIndicesData(2 * m_pointnumpercircle);
		m_LightIndices.PushIndicesData(i + m_pointnumpercircle);
		m_LightIndices.PushIndicesData(2 * m_pointnumpercircle);
	}
}

void LightGizmo::UpdateLight(const LightComponent* lc)
{
	if (!m_isCreateFromNode)
	{
		return;
	}
	RenderComponent* rc = m_pLightNode->TryGetComponent<RenderComponent>();
	if (m_eCurrentLightType != lc->GetLightType())
	{
		m_eCurrentLightType = lc->GetLightType();
		m_LightVertex.Clear();
		m_LightIndices.Clear();
		m_LightVertex.ReserveBuffer(0);
		m_LightIndices.ReserveBuffer(0);
		SubMesh* dc = rc->GetSubMeshByIndex(0);
		switch (m_eCurrentLightType)
		{
		case GraphicDefine::LT_DIRECTIONAL:
		{
			m_GizmoPath = GizmoConfig::DIRECTION_LIGHT_TEX_PATH;
			SetTexture(m_GizmoPath);
			CreateDirectionLight(lc);
			dc->SetDrawRange(0, m_pointnumpercircle * 4);
			break;
		}
		case GraphicDefine::LT_POINT:
		{
			m_GizmoPath = GizmoConfig::POINT_LIGHT_TEX_PATH;
			SetTexture(m_GizmoPath);
			CreatePointLight(lc);
			dc->SetDrawRange(0, m_pointnumpercircle * 6);
			break;
		}
		case GraphicDefine::LT_SPOT:
		{
			m_GizmoPath = GizmoConfig::SPOT_LIGHT_TEX_PATH;
			SetTexture(m_GizmoPath);
			CreateSpotLight(lc);
			dc->SetDrawRange(0, m_pointnumpercircle * 8);
			break;
		}
		default:
		{
			m_GizmoPath = GizmoConfig::AMBIENT_LIGHT_TEX_PATH;
			SetTexture(m_GizmoPath);
			dc->SetDrawRange(0, 0);
			break;
		}
		}
		rc->ChangeVertexBuffer(m_LightVertex);
		rc->ChangeIndexBuffer(m_LightIndices);
		return;

	}

	uint vsindex = m_LightVertex.GetAttributeIndex(RHIDefine::PS_ATTRIBUTE_POSITION);
	switch (m_eCurrentLightType)
	{
	case GraphicDefine::LT_DIRECTIONAL:UpdateDirectionLight(vsindex, lc); break;
	case GraphicDefine::LT_POINT:UpdatePointLight(vsindex, lc); break;
	case GraphicDefine::LT_SPOT:UpdateSpotLight(vsindex, lc); break;
	default: break;
	}


	rc->ChangeVertexBuffer(m_LightVertex);
}

void LightGizmo::UpdateDirectionLight(uint vsindex, const LightComponent* lc)
{
	const Math::Vec3& lightpos = lc->GetWorldPosition();
	const Math::Vec3& lightup = lc->GetWorldUp();
	const Math::Vec3& lightdir = lc->GetWorldDirection();
	const Math::Vec3&& lightright = lightup.Cross(lightdir);
	std::vector<Math::Vec3>&& tablez = GetCirclePoint(lightpos, lightright, lightup, m_pointnumpercircle, 0.1);
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.ChangeVertexDataWithAttributeFast(vsindex, i, tablez[i].GetPtr());

	}
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.ChangeVertexDataWithAttributeFast(vsindex, i + m_pointnumpercircle, (tablez[i] + lightdir).GetPtr());

	}
	m_LightVertex.SetReflushInterval(0, m_pointnumpercircle * 2 - 1);
}

void LightGizmo::UpdatePointLight(uint vsindex, const LightComponent* lc)
{
	const Math::Vec3& lightpos = lc->GetWorldPosition();
	Math::Vec3 lightup(0, 1, 0);
	Math::Vec3 lightdir(0, 0, 1);
	Math::Vec3 lightright(1, 0, 0);
	float range = lc->GetRange();
	std::vector<Math::Vec3>&& tablex = GetCirclePoint(lightpos, lightup, lightdir, m_pointnumpercircle, range);
	std::vector<Math::Vec3>&& tabley = GetCirclePoint(lightpos, lightright, lightdir, m_pointnumpercircle, range);
	std::vector<Math::Vec3>&& tablez = GetCirclePoint(lightpos, lightright, lightup, m_pointnumpercircle, range);
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.ChangeVertexDataWithAttributeFast(vsindex, i, tablex[i].GetPtr());

	}
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.ChangeVertexDataWithAttributeFast(vsindex, i + m_pointnumpercircle, tabley[i].GetPtr());

	}
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.ChangeVertexDataWithAttributeFast(vsindex, i + 2 * m_pointnumpercircle, tablez[i].GetPtr());

	}
	m_LightVertex.SetReflushInterval(0, m_pointnumpercircle * 3 - 1);
}

void LightGizmo::UpdateSpotLight(uint vsindex, const LightComponent* lc)
{
	const Math::Vec3& lightpos = lc->GetWorldPosition();
	const Math::Vec3& lightup = lc->GetWorldUp();
	const Math::Vec3& lightdir = lc->GetWorldDirection();
	const Math::Vec3&& lightright = lightup.Cross(lightdir);
	Math::Vec2 range = lc->GetLightAngle();
	float innerangle = range.x / 2.0;
	float outerangle = range.y / 2.0;
	std::vector<Math::Vec3>&& tablein = GetCirclePoint(lightpos + lightdir * lc->GetRange(), lightup * lc->GetRange(), lightright * lc->GetRange(), m_pointnumpercircle, tan(innerangle));
	std::vector<Math::Vec3>&& tableout = GetCirclePoint(lightpos + lightdir * lc->GetRange(), lightup * lc->GetRange(), lightright * lc->GetRange(), m_pointnumpercircle, tan(outerangle));
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.ChangeVertexDataWithAttributeFast(vsindex, i, tablein[i].GetPtr());
	}
	for (int i = 0; i < m_pointnumpercircle; i++)
	{
		m_LightVertex.ChangeVertexDataWithAttributeFast(vsindex, i + m_pointnumpercircle, tableout[i].GetPtr());
	}
	m_LightVertex.ChangeVertexDataWithAttributeFast(vsindex, 2 * m_pointnumpercircle, lightpos.GetPtr());
	m_LightVertex.SetReflushInterval(0, m_pointnumpercircle * 2);
}

void LightGizmo::SetLightShow(bool show)
{
	RenderComponent* rc = m_pLightNode->TryGetComponent<RenderComponent>();
	if (show)
	{
		rc->SetRenderProperty(GraphicDefine::RP_SHOW);
	}
	else
	{
		rc->EraseRenderProperty(GraphicDefine::RP_SHOW);
	}
}

std::vector<Math::Vec3> LightGizmo::GetCirclePoint(const Math::Vec3& pos, const Math::Vec3& up, const Math::Vec3& right, int pointnum, float size)
{
	float theta = Math::PI * 2.0 / pointnum;
	std::vector<Math::Vec3> pointset;
	for (int i = 0; i < pointnum; i++)
	{
		Math::Vec3 point = pos + up * (Math::Sin(i * theta) * size) + right * (Math::Cos(i * theta) * size);
		pointset.push_back(point);
	}
	return pointset;
}

void LightGizmo::SetTexture(const std::string& str)
{
	TextureEntity* tex = _NEW TextureEntity();
	tex->PushMetadata(TextureFileMetadata(
		RHIDefine::TextureType::TEXTURE_2D,
		RHIDefine::TU_STATIC,
		RHIDefine::PF_AUTO, 1, false, 0,
		RHIDefine::TW_CLAMP_TO_EDGE,
		RHIDefine::TW_CLAMP_TO_EDGE,
		RHIDefine::TF_NEAREST,
		RHIDefine::TF_NEAREST,
		str));
	tex->CreateResource();
	RenderComponent* quadrc = m_pGizmoNode->TryGetComponent<RenderComponent>();
	quadrc->SetParameter(RHIDefine::PS_TEXTURE_DIFFUSE, MakeMaterialParam(tex));
}

void LightGizmo::SetGizmoActive(bool isActive)
{
	m_pGizmoNode->SetActive(isActive);
}

bool LightGizmo::GetGizmoActive()
{
	return m_pGizmoNode->isActive();
}

NS_JYE_END