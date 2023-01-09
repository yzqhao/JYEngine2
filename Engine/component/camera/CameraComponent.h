#pragma once

#include "Engine/private/Define.h"
#include "Engine/component/Component.h"
#include "Engine/GraphicDefine.h"
#include "Math/3DMath.h"
#include "Engine/pipeline/RenderPipe.h"
#include "System/Delegate.h"
#include "CameraGizmo.h"

NS_JYE_BEGIN

class IWindow;
class Scene;
class RenderContext;
class RenderTargetEntity;
class IMessage;

class ENGINE_API CameraComponent : public Component
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	DECLARE_MULTICAST_DELEGATE(ResizeviewCallbackList, int, int, int, int, int, int);
public:
	CameraComponent() = delete;
	CameraComponent(GObject* pHostNode);
	virtual ~CameraComponent();

	virtual void BeforeDelete();
	virtual void OnRootNodeChange();

	FORCEINLINE const Math::FLinearColor& GetClearColor();
	FORCEINLINE void SetClearColor(const Math::FLinearColor& c);
	FORCEINLINE GraphicDefine::CameraProjectionType GetProjType() const;
	FORCEINLINE void SetProjType(GraphicDefine::CameraProjectionType tProj);
	FORCEINLINE Math::IntVec2 GetCameraResolution();
	FORCEINLINE float GetFov() const;
	FORCEINLINE void SetFov(float fov);
	FORCEINLINE float GetNear() const;
	FORCEINLINE void SetNear(float near);
	FORCEINLINE float GetFar() const;
	FORCEINLINE void SetFar(float far);
	FORCEINLINE const Math::Vec3& GetForward() const;
	FORCEINLINE const Math::Vec3& GetPosition() const;
	FORCEINLINE const Math::Vec3& GetLookAt() const;
	FORCEINLINE const Math::Vec3& GetUp() const;

	FORCEINLINE const Math::Mat4& GetProject() const;
	FORCEINLINE const Math::Mat4& GetViewProj() const;
	FORCEINLINE const Math::Mat4& GetView() const;
	FORCEINLINE const Math::Mat4& GetUnViewProj() const;
	FORCEINLINE const Math::Mat4& GetUnView() const;
	FORCEINLINE const Math::Mat4& GetUnProject() const;

	FORCEINLINE void AddLayerMask(uint64 l);
	FORCEINLINE void EraseLayerMask(uint64 l);
	FORCEINLINE void SetLayerMaskNothing();
	FORCEINLINE void SetLayerMaskEverything();
	FORCEINLINE bool isLayerMask(uint64 l);
	FORCEINLINE uint64 GetLayerMask();
	FORCEINLINE void SetLayerMask(uint64 l);
	FORCEINLINE void SetSequence(int q);
	FORCEINLINE int GetSequence();
	FORCEINLINE void SetSequenceCulling(bool is);
	FORCEINLINE bool isSequenceCulling();
	FORCEINLINE RHIDefine::ClearFlag GetClearFlag();
	FORCEINLINE void SetClearFlag(RHIDefine::ClearFlag c);

	FORCEINLINE bool AttachRenderTarget(RenderTargetEntity* rt);
	FORCEINLINE void DetachRenderTarget();
	FORCEINLINE IWindow* GetRenderWindow();
	FORCEINLINE RenderTargetEntity* GetAttachedRenderTarget() const;
	FORCEINLINE void CreateUICameraProjection(float x, float y, float w, float h);
	FORCEINLINE const Math::Frustum& GetFrustum() const;	//得到视锥

	FORCEINLINE float GetShadowBias() const;
	FORCEINLINE void SetShadowBias(float shadowBias);
	FORCEINLINE const Math::Vec3& LineDepthParam() const;//都是平方值
	FORCEINLINE const Math::Vec3& LineDepthParamBais() const;//都是平方值

	void ChangeResolution(const Math::IntVec2& resolution);
	void LookAt(const Math::Vec3& position, const Math::Vec3& look, const Math::Vec3& up);

	void SetPosition(const Math::Vec3& position);
	void SetForward(const Math::Vec3& forward);
	void SetUp(const Math::Vec3& up);

	void CreateProjection();
	void CreatePerspectiveProjection(float nearClip, float farClip);
	void CreatePerspectiveProjection(const Math::IntVec2& size, float nearClip, float farClip);
	void CreatePerspectiveProjection(float radian, float aspect, float nearClip, float farClip);
	void CreateOrthographiProjection(float w, float h, float n, float f);

	void Recalculate();

	void SetPipelineType(GraphicDefine::PiplineType type);
	void SetRenderWindow(IWindow* w);
	void OnResizeview(int bx, int by, int w, int h, int vw, int vh);

	int Render(RenderContext& contex);

	Math::Ray PickRay(const Math::Vec2& secen);
	Vector<GObject*> PickNodes(const Math::Vec2& pos);

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLateLoad(Stream* pStream) override;

	void MessageLoaclPosition(const IMessage& message);
	void MessageLoaclRotation(const IMessage& message); 

#ifdef _EDITOR
	void SetFrustumShow(bool show);//脚本调用
	void SetGizmoShow(bool show);
	void SetGizmoActive(bool isActive);
	bool GetGizmoActive();
	void CreateFrustumResource(); //NodeWrap调用
#endif

protected:

	virtual void _OnUpdate(float dt);
	virtual void _DoOnAttachNode() override;
	virtual void _DoOnDetachNode() override;
	virtual void _OnAwake() override;

	void _OnPositionChange();
	void _OnRotationChange();
	void _NearFar(float nearClip, float farClip);

	RHIDefine::ClearFlag m_eClearFlag;
	GraphicDefine::PiplineType m_ePipelineType;
	Math::FLinearColor m_ClearColor;
	Scene* m_Scene;
	RenderPipe* m_pIRenderPipe;
	IWindow* m_rpIWindow;
	uint64 m_LayerMask;
	Math::IntVec2 m_ChangeResolution;
	ResizeviewCallbackListDelegate m_ResizeFunction;
	DelegateHandle m_ResizeFunctionHandle;

	GraphicDefine::CameraProjectionType m_ProjectionType;
	Math::IntVec2 m_CameraResolution;
	Math::Mat4 m_View;//视图矩阵
	Math::Mat4 m_Project; //投影矩阵
	Math::Mat4 m_ViewProject;//视图投影视图
	mutable Math::Mat4 m_UnView;//view的逆矩阵
	mutable Math::Mat4 m_UnProject;//project的逆矩阵
	mutable Math::Mat4 m_UnViewProject;//view,project的逆矩阵
	mutable Math::Frustum m_Frustum;
	Math::Vec3 m_Position;//位置
	Math::Vec3 m_LookAt;//看向的点
	Math::Vec3 m_Up;//上轴
	Math::Vec3 m_ViewDirction;
	float m_Fov_Size;
	float m_Near;
	float m_Far;
	int m_Sequence;
	bool m_isSequenceCulling;

	DelegateHandle m_hMessagePosition;
	DelegateHandle m_hMessageRotation;

#ifdef _EDITOR//为了剔除不希望再scene中显示的node（摄像头），独立增加的功能
	CameraGizmo* m_CameraGizmo;
#endif

	bool m_isChange;	// 摄像机有变化
	mutable bool m_isDirtyRotation;
	mutable bool m_isDirtyTransform;
	mutable bool m_isDirtyUnView;
	mutable bool m_isDirtyUnProject;
	mutable bool m_isDirtyUnViewProject;
	mutable bool m_isDirtyFrustum;//视锥相关

	float m_shadowBias;
	Math::Vec3 m_LineDepthParam;//线性深度的参数
	Math::Vec3 m_LineDepthParamBias;//线性深度的参数,带偏移的
};
DECLARE_Ptr(CameraComponent);
TYPE_MARCO(CameraComponent);

FORCEINLINE const Math::FLinearColor& CameraComponent::GetClearColor()
{
	return m_ClearColor;
}
FORCEINLINE void CameraComponent::SetClearColor(const Math::FLinearColor& c)
{
	m_ClearColor = c;
}

FORCEINLINE GraphicDefine::CameraProjectionType CameraComponent::GetProjType() const
{
	return m_ProjectionType;
}

FORCEINLINE void CameraComponent::SetProjType(GraphicDefine::CameraProjectionType tProj)
{
	m_ProjectionType = tProj;
}

FORCEINLINE Math::IntVec2 CameraComponent::GetCameraResolution()
{
	return m_CameraResolution;
}

FORCEINLINE float CameraComponent::GetNear() const
{
	return m_Near;
}

FORCEINLINE void CameraComponent::SetNear(float vnear)
{
	m_Near = vnear;
}

FORCEINLINE float CameraComponent::GetFar() const
{
	return m_Far;
}

FORCEINLINE void CameraComponent::SetFar(float vfar)
{
	m_Far = vfar;
}

FORCEINLINE float CameraComponent::GetFov() const
{
	return m_Fov_Size;
}

FORCEINLINE void CameraComponent::SetFov(float fov)
{
	m_Fov_Size = fov;
}

FORCEINLINE const Math::Vec3& CameraComponent::GetForward() const
{
	return m_ViewDirction;
}

FORCEINLINE const Math::Vec3& CameraComponent::GetPosition() const
{
	return m_Position;
}

FORCEINLINE const Math::Vec3& CameraComponent::GetLookAt() const
{
	return m_LookAt;
}

FORCEINLINE const Math::Vec3& CameraComponent::GetUp() const
{
	return m_Up;
}


FORCEINLINE const Math::Mat4& CameraComponent::GetProject() const
{
	return m_Project;
}

FORCEINLINE const Math::Mat4& CameraComponent::GetViewProj() const
{
	return m_ViewProject;
}

FORCEINLINE const Math::Mat4& CameraComponent::GetView() const
{
	return m_View;
}

FORCEINLINE const Math::Mat4& CameraComponent::GetUnViewProj() const
{
	if (m_isDirtyUnViewProject)
	{
		m_isDirtyUnViewProject = false;
		m_UnViewProject = GetUnProject() * GetUnView();
	}
	return m_UnViewProject;
}

FORCEINLINE const Math::Mat4& CameraComponent::GetUnView() const
{
	if (m_isDirtyUnView)
	{
		m_isDirtyUnView = false;
		m_UnView = GetView().GetInversed();
	}
	return m_UnView;
}

FORCEINLINE const Math::Mat4& CameraComponent::GetUnProject() const
{
	if (m_isDirtyUnProject)
	{
		m_isDirtyUnProject = false;
		m_UnProject = GetProject().GetInversed();
	}
	return m_UnProject;
}

FORCEINLINE void CameraComponent::AddLayerMask(uint64 l)
{
	m_LayerMask |= l;
}

FORCEINLINE void CameraComponent::EraseLayerMask(uint64 l)
{
	m_LayerMask &= ~l;
}

FORCEINLINE void CameraComponent::SetLayerMaskNothing()
{
	m_LayerMask = 0;
}

FORCEINLINE void CameraComponent::SetLayerMaskEverything()
{
	m_LayerMask = ~0;
}

FORCEINLINE bool CameraComponent::isLayerMask(uint64 l)
{
	return 0 != (m_LayerMask & l);
}

FORCEINLINE uint64 CameraComponent::GetLayerMask()
{
	return m_LayerMask;
}

FORCEINLINE void CameraComponent::SetLayerMask(uint64 l)
{
	m_LayerMask = l;
}

FORCEINLINE void CameraComponent::SetSequence(int q)
{
	m_Sequence = q;
}

FORCEINLINE int CameraComponent::GetSequence()
{
	return m_Sequence;
}

FORCEINLINE void CameraComponent::SetSequenceCulling(bool is)
{
	m_isSequenceCulling = is;
}

FORCEINLINE bool CameraComponent::isSequenceCulling()
{
	return m_isSequenceCulling;
}

FORCEINLINE RHIDefine::ClearFlag CameraComponent::GetClearFlag()
{
	return m_eClearFlag;
}

FORCEINLINE void CameraComponent::SetClearFlag(RHIDefine::ClearFlag c)
{
	m_eClearFlag = c;
}

FORCEINLINE bool CameraComponent::AttachRenderTarget(RenderTargetEntity* rt)
{
	return m_pIRenderPipe->AttachRenderTarget(rt);
}

FORCEINLINE void CameraComponent::DetachRenderTarget() 
{
	m_pIRenderPipe->DetachRenderTarget();
}

FORCEINLINE IWindow* CameraComponent::GetRenderWindow()
{
	return m_rpIWindow;
}

FORCEINLINE RenderTargetEntity* CameraComponent::GetAttachedRenderTarget() const
{
	return m_pIRenderPipe->GetAttachedRenderTarget();
}

FORCEINLINE void CameraComponent::CreateUICameraProjection(float x, float y, float w, float h)
{
	m_ProjectionType = GraphicDefine::CPT_UICAMERA;
	float L = x;
	float R = x + w;
	float T = y;
	float B = y + h;
	m_Project = Math::Mat4(
		2.0f / (R - L), 0.0f, 0.0f, (R + L) / (L - R),
		0.0f, 2.0f / (T - B), 0.0f, (T + B) / (B - T),
		0.0f, 0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	m_isDirtyUnProject = true;
	m_isDirtyUnViewProject = true;
	m_isChange = true;
}

FORCEINLINE const Math::Frustum& CameraComponent::GetFrustum() const	//得到视锥
{
	if (m_isDirtyFrustum)
	{
		m_isDirtyFrustum = false;
		m_Frustum.FromMatrix(GetViewProj(), GetUnViewProj());
	}
	return m_Frustum;
}

FORCEINLINE float CameraComponent::GetShadowBias() const
{
	return m_shadowBias;
}

FORCEINLINE void CameraComponent::SetShadowBias(float shadowBias)
{
	m_shadowBias = shadowBias;
}
	
FORCEINLINE const Math::Vec3& CameraComponent::LineDepthParam() const
{
	//线性深度公式 d = ( z - m_LineDepthParam.m_x ) * m_LineDepthParam.m_y;
	//返回实际距离 z = d * m_LineDepthParam.m_z + m_LineDepthParam.m_x;
	return m_LineDepthParam;
}
	
FORCEINLINE const Math::Vec3& CameraComponent::LineDepthParamBais() const
{
	return m_LineDepthParamBias;
}

NS_JYE_END