#pragma once

#include "Engine/GraphicDefine.h"
#include "Engine/render/VertexStream.h"
#include "Engine/render/IndicesStream.h"
#include "Math/3DMath.h"

NS_JYE_BEGIN

class CameraComponent;
class GObject;

class CameraGizmo
{
private:
	VertexStream            m_FrustumVertex;
	VertexStream			m_QuadVertex;
	IndicesStream           m_FrustumIndices;
	IndicesStream			m_QuadIndices;
	GObject* m_pFrustumNode;
	GObject* m_pGizmoNode;
	bool                    m_isCreateFromNode;
public:
	CameraGizmo();
	~CameraGizmo();
	void CreateSource(const Math::Frustum& frustum, const CameraComponent* camera); //����ͼ�����׶��
	void UpdateSource(const Math::Frustum& frustum); //������׶��
	void SetFrustumShow(bool show);

	void SetGizmoShow(bool show);
	/**
	* Retrive the camera icon gizmo's activity
	*/
	bool GetGizmoActive();
	/**
	* Change the camera icon gizmo's activity
	*/
	void SetGizmoActive(bool isActive);
};

NS_JYE_END