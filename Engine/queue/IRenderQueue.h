#pragma once

#include "Engine/private/Define.h"
#include "Engine/GraphicDefine.h"
#include "Math/3DMath.h"

NS_JYE_BEGIN

class RenderComponent;
class CameraComponent;
class GObject;

class ENGINE_API IRenderQueue
{
	friend class RenderComponent;
	typedef std::map< uint64, RenderComponent* > RenderComponentMap;
public:
	IRenderQueue();
	virtual ~IRenderQueue();

	void Update();

	void AttachRenderComponent(RenderComponent* rc);
	void DetachRenderComponent(RenderComponent* rc);

	void OnCulling(uint64 mask, int seq, bool scull, GraphicDefine::PiplineType pp, CameraComponent* cam);
	Vector<GObject*> PickNodes(uint64 mask, int seq, bool scull, const Math::Ray& rhs);

	FORCEINLINE const GraphicDefine::VisableRender& GetVisableRender()
	{
		return m_rpRendering;
	}
	FORCEINLINE Math::AABB GetBoundBox() const
	{
		return m_SceneBoundBox;
	}

protected:

	RenderComponentMap m_pAttachedComponentMap;
	GraphicDefine::VisableRender m_rpRendering;
	Math::AABB m_SceneBoundBox;  //³¡¾°°üÎ§ºÐ
};

NS_JYE_END