
#include "CameraComponent.h"
#include "Engine/object/GObject.h"
#include "Engine/object/Scene.h"
#include "Engine/pipeline/RenderPipe.h"
#include "Engine/ViewResolution.h"
#include "Core/Interface/IWindow.h"
#include "Core/Interface/IApplication.h"
#include "Engine/render/RenderTargetEntity.h"
#include "Engine/object/IMessage.h"
#include "Core/Configure.h"
#include "Engine/queue/ILightQueue.h"
#include "Engine/queue/IRenderQueue.h"

NS_JYE_BEGIN

#define BAIS 0.98

static void CameraComponent_m_GetNear(CameraComponent* self, float* value)
{
	*value = self->GetNear();
}

static void CameraComponent_m_SetNear(CameraComponent* self, float* value)
{
	self->SetNear(*value);
	self->CreateProjection();
}

static void CameraComponent_m_GetFar(CameraComponent* self, float* value)
{
	*value = self->GetFar();
}

static void CameraComponent_m_SetFar(CameraComponent* self, float* value)
{
	self->SetFar(*value);
	self->CreateProjection();
}

static void CameraComponent_m_GetSequence(CameraComponent* self, int* value)
{
	*value = self->GetSequence();
}

static void CameraComponent_m_SetSequence(CameraComponent* self, int* value)
{
	self->SetSequence(*value);
}

static void CameraComponent_m_GetProjType(CameraComponent* self, GraphicDefine::CameraProjectionType* value)
{
	*value = self->GetProjType();
}

static void CameraComponent_m_SetProjType(CameraComponent* self, GraphicDefine::CameraProjectionType* value)
{
	self->SetProjType(*value);
	self->CreateProjection();
}

static void CameraComponent_m_GetFor(CameraComponent* self, float* value)
{
	*value = self->GetFov();
}

static void CameraComponent_m_SetFor(CameraComponent* self, float* value)
{
	self->SetFov(*value);
	self->CreateProjection();
}

static void CameraComponent_m_GetClearColor(CameraComponent* self, Math::FLinearColor* value)
{
	*value = self->GetClearColor();
}

static void CameraComponent_m_SetClearColor(CameraComponent* self, Math::FLinearColor* value)
{
	self->SetClearColor(*value);
}

static void CameraComponent_m_GetClearFlag(CameraComponent* self, RHIDefine::ClearFlag* value)
{
	*value = self->GetClearFlag();
}

static void CameraComponent_m_SetClearFlag(CameraComponent* self, RHIDefine::ClearFlag* value)
{
	self->SetClearFlag(*value);
}

IMPLEMENT_RTTI(CameraComponent, Component);
BEGIN_ADD_PROPERTY(CameraComponent, Component);
REGISTER_PROPERTY(m_eClearFlag, m_eClearFlag, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_ePipelineType, m_ePipelineType, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_ClearColor, m_ClearColor, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Scene, m_Scene, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_LayerMask, m_LayerMask, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_ProjectionType, m_ProjectionType, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_CameraResolution, m_CameraResolution, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Fov_Size, m_Fov_Size, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Near, m_Near, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Far, m_Far, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_ViewDirction, m_ViewDirction, Property::F_SAVE_LOAD_CLONE)		// ViewDirction和up 序列化出去，减少反序列化顺序带来的问题
REGISTER_PROPERTY(m_Up, m_Up, Property::F_SAVE_LOAD_CLONE)		
REGISTER_PROPERTY(m_Sequence, m_Sequence, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_isSequenceCulling, m_isSequenceCulling, Property::F_SAVE_LOAD_CLONE)
ADD_PROPERTY_MEM_FUNCTION(m_Near, CameraComponent_m_GetNear, CameraComponent_m_SetNear)
ADD_PROPERTY_MEM_FUNCTION(m_Far, CameraComponent_m_GetFar, CameraComponent_m_SetFar)
ADD_PROPERTY_MEM_FUNCTION(m_Sequence, CameraComponent_m_GetSequence, CameraComponent_m_SetSequence)
ADD_PROPERTY_MEM_FUNCTION(m_ProjectionType, CameraComponent_m_GetProjType, CameraComponent_m_SetProjType)
ADD_PROPERTY_MEM_FUNCTION(m_Fov_Size, CameraComponent_m_GetFor, CameraComponent_m_SetFor)
ADD_PROPERTY_MEM_FUNCTION(m_ClearColor, CameraComponent_m_GetClearColor, CameraComponent_m_SetClearColor)
ADD_PROPERTY_MEM_FUNCTION(m_eClearFlag, CameraComponent_m_GetClearFlag, CameraComponent_m_SetClearFlag)
END_ADD_PROPERTY

COMPONENT_FACTORY_FUNC(CameraComponent)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(CameraComponent)
IMPLEMENT_INITIAL_END

// TODO :
// m_pIRenderPipe 这里构造函数默认创建了对象。这个属性也是序列化出去的，在反序列化的时候会造成内存泄漏，这里考虑下怎么处理
CameraComponent::CameraComponent(GObject* pHostNode)
	: Component(pHostNode)
	, m_eClearFlag(RHIDefine::CF_COLOR_DEPTH_STENCIL)
	, m_ePipelineType(GraphicDefine::PP_FORWARD_LIGHTING)
	, m_pIRenderPipe(_NEW RenderPipe())
	, m_ClearColor(Math::FColor::Gray)
	, m_LayerMask(MC_MASK_DEFAULT_LAYER)
	, m_Scene(nullptr)
	, m_rpIWindow(nullptr)
	, m_Fov_Size(0)
	, m_Near(0)
	, m_Far(0)
	, m_Sequence(0)
	, m_isSequenceCulling(false)
	, m_isChange(false)
	, m_isDirtyRotation(false)
	, m_isDirtyTransform(false)
	, m_isDirtyUnView(false)
	, m_isDirtyUnProject(false)
	, m_isDirtyUnViewProject(false)
	, m_isDirtyFrustum(false)
	, m_CameraResolution(16, 16)
{
	SetRenderWindow(IApplication::Instance()->GetMainWindow());

	m_pIRenderPipe->SetPipelinePass(m_ePipelineType);

#ifdef _EDITOR
	m_CameraGizmo = _NEW CameraGizmo;
#endif
}

CameraComponent::~CameraComponent()
{
	SAFE_DELETE(m_pIRenderPipe);
#ifdef _EDITOR
	SAFE_DELETE(m_CameraGizmo);
#endif
}

void CameraComponent::BeforeDelete()
{
	SAFE_DELETE(m_pIRenderPipe);
}

void CameraComponent::OnRootNodeChange()
{
	_OnPositionChange();
	_OnRotationChange();
}

void CameraComponent::ChangeResolution(const Math::IntVec2& resolution)
{
	m_ChangeResolution = resolution;
}

void CameraComponent::LookAt(const Math::Vec3& position, const Math::Vec3& look, const Math::Vec3& up)
{
	Math::Vec3 forward = look - position;
	forward.Normalize();
	Math::Vec3 normalized_up = up.GetNormalized();
	Math::Vec3 perpendicular_up = normalized_up - forward * normalized_up.Dot(forward);
	perpendicular_up.Normalize();

	Math::Vec3 z = forward;
	Math::Vec3 y = perpendicular_up;
	Math::Vec3 x = y.Cross(z).GetNormalized();
	Math::Mat3 mrot(x.x, x.y, x.z,
		y.x, y.y, y.z,
		z.x, z.y, z.z);

	Math::Quaternion rot(mrot);
	GetParentObject()->SendMessage(IMessage(this, &rot, IMessage::SA_ROTATION), IMessage::MT_SELFNODE, IMessage::NEED_AFFECT);
	GetParentObject()->SendMessage(IMessage(this, &position, IMessage::SA_POSITION), IMessage::MT_SELFNODE, IMessage::NEED_AFFECT);

	m_Position = position;
	m_LookAt = look;
	m_Up = up;
	m_ViewDirction = m_LookAt - m_Position;
	m_ViewDirction.Normalize();

	m_isChange = true;
	m_isDirtyFrustum = true;
}

void CameraComponent::_OnUpdate(float dt)
{
	if (m_isChange)
	{
		m_isChange = false;
		//计算新的view矩阵
		m_View = Math::Mat4::CreateLookAt(m_Position, m_LookAt, m_Up);
		m_ViewProject = (m_View) * (m_Project);

		//计算其他矩阵
		m_isDirtyRotation = true;
		m_isDirtyTransform = true;
		m_isDirtyUnView = true;
		m_isDirtyUnProject = true;
		m_isDirtyUnViewProject = true;
		m_isDirtyFrustum = true;
	}
}

void CameraComponent::_DoOnAttachNode()
{
	m_pIRenderPipe->SetHostCamera(this);
	m_pIRenderPipe->SetResolution(GetCameraResolution());
	m_pIRenderPipe->UpdateRenderTarget(m_rpIWindow);

	m_Scene = GetParentObject()->GetScene();
	m_Scene->AttachCamera(this);

	m_hMessagePosition = GetParentObject()->RegisterMessageListener(IMessage::SA_POSITION, MessageDelegateDelegate::CreateRaw(this, &CameraComponent::MessageLoaclPosition));
	m_hMessageRotation = GetParentObject()->RegisterMessageListener(IMessage::SA_SCALE, MessageDelegateDelegate::CreateRaw(this, &CameraComponent::MessageLoaclRotation));

	_OnPositionChange();
	_OnRotationChange();
}

void CameraComponent::_DoOnDetachNode()
{
	m_Scene->DetachCamera(this);
	m_Scene = nullptr;

	if (m_rpIWindow && m_ResizeFunctionHandle.IsValid())
	{
		m_rpIWindow->RemoveResizeviewCallback(m_ResizeFunctionHandle);
		m_ResizeFunction.Clear();
		m_ResizeFunctionHandle.Reset();
	}

	GetParentObject()->RemoveMessageListener(IMessage::SA_POSITION, m_hMessagePosition);
	GetParentObject()->RemoveMessageListener(IMessage::SA_SCALE, m_hMessageRotation);

	_OnPositionChange();
	_OnRotationChange();
}

void CameraComponent::_OnAwake()
{
	m_pIRenderPipe->SetResolution(GetCameraResolution());
	m_pIRenderPipe->SetPipelinePass(m_ePipelineType);
}

void CameraComponent::BeforeSave(Stream* pStream)
{
	Component::BeforeSave(pStream);
}

void CameraComponent::PostSave(Stream* pStream)
{
	Component::PostSave(pStream);
}

void CameraComponent::PostLateLoad(Stream* pStream)
{
	Component::PostLateLoad(pStream);
	CreateProjection();

#ifdef _EDITOR
	CreateFrustumResource();
#endif
}

void CameraComponent::MessageLoaclPosition(const IMessage& message)
{
	if (message.isValidMessage(this))
	{
		_OnPositionChange();
	}
}

void CameraComponent::MessageLoaclRotation(const IMessage& message)
{
	if (message.isValidMessage(this))
	{
		_OnRotationChange();
	}
}

void CameraComponent::_OnPositionChange()
{
	const Math::Vec3* worldPos = GetParentObject()->Attribute<Math::Vec3>(GraphicDefine::GA_WORLD_POSITION);
	if (worldPos)
	{
		SetPosition(*worldPos);
	}
}

void CameraComponent::_OnRotationChange()
{
	const Math::Quaternion* worldRota = GetParentObject()->Attribute<Math::Quaternion>(GraphicDefine::GA_WORLD_ROTATION);
	if (worldRota)
	{
		Math::Vec3 forward = (*worldRota) * Configure::Vector3_DefaultForward;
		Math::Vec3 up = (*worldRota) * Configure::Vector3_DefaultUp;
		forward.Normalize();
		up.Normalize();
		SetUp(up);
		SetForward(forward);
	}
}

void CameraComponent::_NearFar(float nearClip, float farClip)
{
	m_Near = nearClip;
	m_Far = farClip;
	
	m_LineDepthParam.x = m_Near;
	m_LineDepthParam.y = 1.0 / (m_Far - m_Near);
	m_LineDepthParam.z = m_Far - m_Near;

	m_LineDepthParamBias.x = m_Near;
	m_LineDepthParamBias.y = 1.0 / (m_Far * BAIS - m_Near);
	m_LineDepthParamBias.z = m_Far * BAIS - m_Near;
}

void CameraComponent::SetPosition(const Math::Vec3& position)
{
	GetParentObject()->SendMessage(IMessage(this, &position, IMessage::SA_POSITION), IMessage::MT_SELFNODE, IMessage::NEED_AFFECT);
	LookAt(position, position + m_ViewDirction, m_Up);
}

void CameraComponent::SetForward(const Math::Vec3& forward)
{
	GetParentObject()->SendMessage(IMessage(this, &forward, IMessage::SA_ROTATION), IMessage::MT_SELFNODE, IMessage::NEED_AFFECT);
	LookAt(m_Position, m_Position + forward, m_Up);
}

void CameraComponent::SetUp(const Math::Vec3& up)
{
	m_Up = up;
	m_isDirtyFrustum = true;
	m_isChange = true;
}

void CameraComponent::CreateProjection()
{
	switch (m_ProjectionType)
	{
	case GraphicDefine::CPT_PERSPECTIVE:CreatePerspectiveProjection(m_Fov_Size, 1.0f * m_CameraResolution.x / m_CameraResolution.y, m_Near, m_Far);
		break;
	case GraphicDefine::CPT_ORTHOGRAPHI:CreateOrthographiProjection(m_CameraResolution.x, m_CameraResolution.y, m_Near, m_Far);
		break;
	case GraphicDefine::CPT_UICAMERA:
		break;
	default:
		break;
	}
}

void CameraComponent::CreatePerspectiveProjection(float nearClip, float farClip)
{
	CreatePerspectiveProjection(m_CameraResolution, nearClip, farClip);
}

void CameraComponent::CreatePerspectiveProjection(const Math::IntVec2& size, float nearClip, float farClip)
{
	float aspect = 1.0 * size.x / size.y;
	CreatePerspectiveProjection(Math::PI / 4, aspect, nearClip, farClip);
}

void CameraComponent::CreatePerspectiveProjection(float radian, float aspect, float nearClip, float farClip)
{
	m_Fov_Size = radian;
	_NearFar(nearClip, farClip);
	farClip *= BAIS;
	m_Project = Math::Mat4::CreatePerspective(radian, aspect, nearClip, farClip);

	m_isDirtyUnProject = true;
	m_isDirtyUnViewProject = true;
	m_isChange = true;
	_OnUpdate(0);
}

void CameraComponent::CreateOrthographiProjection(float w, float h, float n, float f)
{
	_NearFar(n, f);
	m_Project = Math::Mat4::CreateOrthographic(w, h, n, f);

	f *= BAIS;
	m_isDirtyUnProject = true;
	m_isDirtyUnViewProject = true;
	m_isChange = true;
	_OnUpdate(0);
}

void CameraComponent::Recalculate()
{
	_OnUpdate(0);
#ifdef _EDITOR
	m_CameraGizmo->UpdateSource(GetFrustum());
#endif
}

void CameraComponent::SetPipelineType(GraphicDefine::PiplineType type)
{
	if (m_ePipelineType != type)
	{
		m_ePipelineType = type;
		if (IsAwaked())
		{
			m_pIRenderPipe->SetPipelinePass(type);
		}
	}
}

void CameraComponent::SetRenderWindow(IWindow* w)
{
	if (m_rpIWindow != w)
	{
		if (m_rpIWindow && m_ResizeFunctionHandle.IsValid())
		{
			m_rpIWindow->RemoveResizeviewCallback(m_ResizeFunctionHandle);
			m_ResizeFunction.Clear();
			m_ResizeFunctionHandle.Reset();
		}
		m_rpIWindow = w;
		if (m_rpIWindow)
		{
			int x, y;
			m_rpIWindow->GetWindowSize(&x, &y);
			m_ChangeResolution = Math::IntVec2(x, y);

			if (m_pIRenderPipe)
			{
				m_pIRenderPipe->UpdateRenderTarget(m_rpIWindow);
			}

			m_ResizeFunction = MulticastDelegate<int, int, int, int, int, int>::DelegateT::CreateRaw(this, &CameraComponent::OnResizeview);
			m_ResizeFunctionHandle = m_rpIWindow->SetResizeviewCallback(m_ResizeFunction);
		}
	}
}

void CameraComponent::OnResizeview(int bx, int by, int w, int h, int vw, int vh)
{
	m_CameraResolution = Math::IntVec2(w, h);
	ViewResolution vr(GetCameraResolution());
	float aspect = static_cast<float>(vr.m_Resolution.x) / static_cast<float>(vr.m_Resolution.y);
	switch (GetProjType())
	{
	case GraphicDefine::CPT_PERSPECTIVE: CreatePerspectiveProjection(m_CameraResolution, GetNear(), GetFar()); break;
	case GraphicDefine::CPT_ORTHOGRAPHI: CreateOrthographiProjection(GetFov(), aspect, GetNear(), GetFar()); break;
	case GraphicDefine::CPT_UICAMERA: CreateUICameraProjection(vr.m_ViewSize.x, vr.m_ViewSize.y, vr.m_ViewSize.z, vr.m_ViewSize.w); break;
	default:JYERROR("Can't resize view! Unknown camera projection type");
	}

	if (m_pIRenderPipe)
	{
		//resize pipeline 上的target
		m_pIRenderPipe->OnResizeView(vr);
	}
	else
	{
		JYERROR("m_pIRenderPipe Can't resize view! m_pIRenderPipe = nullptr");
	}
}

int CameraComponent::Render(RenderContext& contex)
{
	if (m_ChangeResolution != Math::IntVec2::ZERO)
	{
		if (m_ChangeResolution != m_CameraResolution)//如果是分辨率变更优先处理分辨率变更
		{
			OnResizeview(0, 0, m_ChangeResolution.x, m_ChangeResolution.y, m_ChangeResolution.x, m_ChangeResolution.y);
		}
		m_ChangeResolution = Math::IntVec2::ZERO;
	}
#ifdef _EDITOR
	m_CameraGizmo->UpdateSource(GetFrustum());
#endif
	return m_pIRenderPipe->Render(m_LayerMask, m_Sequence, m_isSequenceCulling, m_eClearFlag, m_ClearColor, m_Scene, contex);
}

Math::Ray CameraComponent::PickRay(const Math::Vec2& secen)
{
	Recalculate();	//PickRay之前先触发update去更新投影矩阵，避免拿到的射线不对
	
	float zNear = 0.0;
	if (m_ProjectionType == GraphicDefine::CPT_ORTHOGRAPHI)
	{
		zNear = -1.0;
	}
	Math::Vec4 rayb(secen.x, secen.y, zNear, 1.0);
	Math::Vec4 raye(secen.x, secen.y, 1.0, 1.0);
	Math::Vec4 ray2;
	rayb = rayb * GetUnViewProj();
	Math::Vec4 temp = rayb * GetViewProj();
	JY_ASSERT(0 != rayb.w);
	raye *= GetUnViewProj();
	JY_ASSERT(0 != raye.w);
	if (m_ProjectionType != GraphicDefine::CPT_ORTHOGRAPHI)
	{
		rayb /= rayb.w;
		raye /= raye.w;
	}

	ray2 = raye - rayb;
	Math::Vec3 reslut2(ray2.x, ray2.y, ray2.z);
	reslut2.Normalize();
	return Math::Ray(Math::Vec3(rayb.x, rayb.y, rayb.z), reslut2);
}

Vector<GObject*> CameraComponent::PickNodes(const Math::Vec2& pos)
{
	Math::Ray r = PickRay(pos);
	return m_Scene->GetRenderQueue()->PickNodes(m_LayerMask, m_Sequence, m_isSequenceCulling, r);
}

#ifdef _EDITOR

void CameraComponent::SetFrustumShow(bool show)
{
	m_CameraGizmo->SetFrustumShow(show);
}

void CameraComponent::SetGizmoShow(bool show)
{
	m_CameraGizmo->SetGizmoShow(show);
}

bool CameraComponent::GetGizmoActive()
{
	return m_CameraGizmo->GetGizmoActive();
}

void CameraComponent::SetGizmoActive(bool isActive)
{
	m_CameraGizmo->SetGizmoActive(isActive);
}

void CameraComponent::CreateFrustumResource()
{
	m_CameraGizmo->CreateSource(GetFrustum(), this);
}
#endif

NS_JYE_END