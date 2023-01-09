#pragma once

#include "Engine/private/Define.h"
#include "Math/3DMath.h"
#include "Engine/GraphicDefine.h"
#include "Engine/render/VertexStream.h"
#include "Engine/render/IndicesStream.h"

NS_JYE_BEGIN

class GObject;

class LightGizmo
{
private:
	GraphicDefine::LightType		m_eCurrentLightType;
	VertexStream					m_LightVertex;
	VertexStream					m_QuadVertex;
	IndicesStream					m_LightIndices;
	IndicesStream					m_QuadIndices;
	GObject* m_pHost;
	GObject* m_pLightNode;
	GObject* m_pGizmoNode;
	String							m_GizmoPath;
	int								m_pointnumpercircle;
	bool                            m_isCreateFromNode;
public:
	LightGizmo();
	~LightGizmo();
	void CreateLight(const LightComponent* lc);
	void CreateDirectionLight(const LightComponent* lc);
	void CreatePointLight(const LightComponent* lc);
	void CreateSpotLight(const LightComponent* lc);
	void UpdateLight(const LightComponent* lc);
	void UpdateDirectionLight(uint vsindex, const LightComponent* lc);
	void UpdatePointLight(uint vsindex, const LightComponent* lc);
	void UpdateSpotLight(uint vsindex, const LightComponent* lc);
	void SetLightShow(bool show);
	void SetTexture(const std::string& str);//反射面板更换光照类型时贴图更新
	std::vector<Math::Vec3> GetCirclePoint(const Math::Vec3& pos, const Math::Vec3& up, const Math::Vec3& right, int pointnum, float size);
	/**
	* Retrive the light icon gizmo's activity
	*/
	bool GetGizmoActive();
	/**
	* Change the light icon gizmo's activity
	*/
	void SetGizmoActive(bool isActive);
};

NS_JYE_END