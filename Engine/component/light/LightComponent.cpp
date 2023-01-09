
#include "LightComponent.h"
#include "LightInfo.h"
#include "Engine/object/GObject.h"
#include "Engine/object/Scene.h"
#include "Engine/queue/ILightQueue.h"
#include "Engine/object/IMessage.h"
#include "Engine/component/camera/CameraComponent.h"
#include "Engine/render/RenderTargetEntity.h"
#include "Engine/resource/RenderTargetMetadata.h"
#include "Engine/render/texture/TextureEntity.h"
#include "Engine/resource/TextureMetadata.h"
#include "Core/Configure.h"
#include "Engine/ProjectSetting.h"
#include "Engine/queue/IRenderQueue.h"

NS_JYE_BEGIN

static void LightComponent_m_GetLightType(LightComponent* self, GraphicDefine::LightType* value)
{
	*value = self->GetLightType();
}

static void LightComponent_m_SetLightType(LightComponent* self, GraphicDefine::LightType* value)
{
	self->Setup(*value);
}

static void LightComponent_m_GetColor(LightComponent* self, Math::Vec3* value)
{
	*value = self->GetColor();
}

static void LightComponent_m_SetColor(LightComponent* self, Math::Vec3* value)
{
	self->SetColor(*value);
}

static void LightComponent_m_GetLightAngle(LightComponent* self, Math::Vec2* value)
{
	*value = self->GetLightAngle();
}

static void LightComponent_m_SetLightAngle(LightComponent* self, Math::Vec2* value)
{
	self->SetLightAngle(*value);
}

static void LightComponent_m_GetRange(LightComponent* self, float* value)
{
	*value = self->GetRange();
}

static void LightComponent_m_SetRange(LightComponent* self, float* value)
{
	self->SetRange(*value);
}

static void LightComponent_m_GetIntensity(LightComponent* self, float* value)
{
	*value = self->GetIntensity();
}

static void LightComponent_m_SetIntensity(LightComponent* self, float* value)
{
	self->SetIntensity(*value);
}

static void LightComponent_m_GetAttenuation(LightComponent* self, Math::Vec4* value)
{
	*value = self->GetAttenuation();
}

static void LightComponent_m_SetAttenuation(LightComponent* self, Math::Vec4* value)
{
	self->SetAttenuation(*value);
}

static void LightComponent_m_GetShadowType(LightComponent* self, GraphicDefine::ShadowType* value)
{
	*value = self->GetShadowType();
}

static void LightComponent_m_SetShadowType(LightComponent* self, GraphicDefine::ShadowType* value)
{
	self->SetShadowType(*value);
}

static void LightComponent_m_GetSoftShadowType(LightComponent* self, GraphicDefine::SoftShadowType* value)
{
	*value = self->GetSoftShadowType();
}

static void LightComponent_m_SetSoftShadowType(LightComponent* self, GraphicDefine::SoftShadowType* value)
{
	self->SetSoftShadowType(*value);
}

static void LightComponent_m_GetShadowBais(LightComponent* self, float* value)
{
	*value = self->GetShadowBais();
}

static void LightComponent_m_SetShadowBais(LightComponent* self, float* value)
{
	self->SetShadowBais(*value);
}

static void LightComponent_m_GetShadowStrength(LightComponent* self, float* value)
{
	*value = self->GetShadowStrength();
}

static void LightComponent_m_SetShadowStrength(LightComponent* self, float* value)
{
	self->SetShadowStrength(*value);
}

IMPLEMENT_RTTI(LightComponent, Component);
BEGIN_ADD_PROPERTY(LightComponent, Component);
REGISTER_PROPERTY(m_eLightType, m_eLightType, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Color, m_Color, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_LightAngle, m_LightAngle, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_LightRange, m_LightRange, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Intensity, m_Intensity, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_Attenuation, m_Attenuation, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_shadowType, m_shadowType, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_softShadowType, m_softShadowType, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_shadowBais, m_shadowBais, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_shadowStrength, m_shadowStrength, Property::F_SAVE_LOAD_CLONE)
ADD_PROPERTY_MEM_FUNCTION(m_eLightType, LightComponent_m_GetLightType, LightComponent_m_SetLightType)
ADD_PROPERTY_MEM_FUNCTION(m_Color, LightComponent_m_GetColor, LightComponent_m_SetColor)
ADD_PROPERTY_MEM_FUNCTION(m_LightAngle, LightComponent_m_GetLightAngle, LightComponent_m_SetLightAngle)
ADD_PROPERTY_MEM_FUNCTION(m_LightRange, LightComponent_m_GetRange, LightComponent_m_SetRange)
ADD_PROPERTY_MEM_FUNCTION(m_Intensity, LightComponent_m_GetIntensity, LightComponent_m_SetIntensity)
ADD_PROPERTY_MEM_FUNCTION(m_Attenuation, LightComponent_m_GetAttenuation, LightComponent_m_SetAttenuation)
ADD_PROPERTY_MEM_FUNCTION(m_shadowType, LightComponent_m_GetShadowType, LightComponent_m_SetShadowType)
ADD_PROPERTY_MEM_FUNCTION(m_softShadowType, LightComponent_m_GetSoftShadowType, LightComponent_m_SetSoftShadowType)
ADD_PROPERTY_MEM_FUNCTION(m_shadowBais, LightComponent_m_GetShadowBais, LightComponent_m_SetShadowBais)
ADD_PROPERTY_MEM_FUNCTION(m_shadowStrength, LightComponent_m_GetShadowStrength, LightComponent_m_SetShadowStrength)
END_ADD_PROPERTY

COMPONENT_FACTORY_FUNC(LightComponent)
IMPLEMENT_INITIAL_NO_FACTORY_BEGIN(LightComponent)
IMPLEMENT_INITIAL_END

LightComponent::LightComponent(GObject* pHostNode)
	: Component(pHostNode)
	, m_eLightType(GraphicDefine::LightType::LT_AMBIENT)
	, m_pScene(nullptr)
	, m_Color(0.2, 0.2, 0.2)
	, m_LightRange(1.0f)
	, m_Intensity(1.0f)
	, m_LightAngle(Math::PI / 4, Math::PI / 3)
	, m_Attenuation(1, 0, 0, 0)
	, m_isLightChange(false)
	, m_pCameraProxy(nullptr)
	, m_pShadowMapCamera(nullptr)
	, m_pLightEntity(nullptr)
	, m_pRenderTargetEntity(nullptr)
	, m_pShadowColorMap(nullptr)
	, m_shadowType(GraphicDefine::NO_SHADOW)
	, m_softShadowType(GraphicDefine::SST_PCF3X3)
	, m_isCastShadow(false)
	, m_shadowBais(0.025f)
	, m_shadowStrength(1.0f)
{
	SetLightAngle(m_LightAngle);
#ifdef _EDITOR
	m_LightGizmo = _NEW LightGizmo();
#endif
}

LightComponent::~LightComponent()
{
	SAFE_DELETE(m_pLightEntity);
	SAFE_DELETE(m_pRenderTargetEntity);
#ifdef _EDITOR
	SAFE_DELETE(m_LightGizmo);
#endif

	if (m_pCameraProxy != NULL)
	{
		m_pScene->DeleteGObject(m_pCameraProxy);
	}
}

void LightComponent::BeforeDelete()
{

}

void LightComponent::OnRootNodeChange()
{

}

void LightComponent::BeforeSave(Stream* pStream)
{
	Component::PostLoad(pStream);
}

void LightComponent::PostSave(Stream* pStream)
{
	Component::PostSave(pStream);
}

void LightComponent::PostLateLoad(Stream* pStream)
{
	Component::PostLateLoad(pStream);
	Setup(m_eLightType);

#ifdef _EDITOR
	CreateLightResource();
#endif
}

void LightComponent::Setup(GraphicDefine::LightType lt)
{
	m_eLightType = lt;
	m_isLightChange = true;
	if (m_pLightEntity)
	{
		SAFE_DELETE(m_pLightEntity);
	}
	switch (m_eLightType)
	{
	case GraphicDefine::LT_AMBIENT: m_pLightEntity = _NEW AmbientLight(this); break;
	case GraphicDefine::LT_DIRECTIONAL: m_pLightEntity = _NEW DirectionLightInfo(this); break;
	case GraphicDefine::LT_POINT: m_pLightEntity = _NEW PointLightInfo(this); break;
	case GraphicDefine::LT_SPOT: m_pLightEntity = _NEW SpotLightInfo(this); break;
	default:JYERROR("Light component setup error : unknown light type");
	}
	SetShadowType(m_shadowType);
}

void LightComponent::MessageLoaclPosition(const IMessage& message)
{
	m_isLightChange = true;
}

void LightComponent::MessageLoaclScale(const IMessage& message)
{
	m_isLightChange = true;
}

void LightComponent::MessageLoaclRotation(const IMessage& message)
{
	m_isLightChange = true;
}

bool LightComponent::TryRecaculateLight()
{
	if (m_isLightChange && m_pLightEntity)
	{
		m_isLightChange = false;
		Math::Quaternion rot;
		Math::Vec3 ws;
		Math::Vec3 wp;
		if (GetParentObject())
		{
			const Math::Quaternion* pq = GetParentObject()->Attribute<Math::Quaternion>(GraphicDefine::GA_WORLD_ROTATION);
			const Math::Vec3* ps = GetParentObject()->Attribute<Math::Vec3>(GraphicDefine::GA_WORLD_SCALE);
			const Math::Vec3* pw = GetParentObject()->Attribute<Math::Vec3>(GraphicDefine::GA_WORLD_POSITION);
			rot = pq ? *pq : rot;
			ws = ps ? *ps : ws;
			wp = pw ? *pw : wp;
		}
		m_WorldPosition = wp;
		m_MaxScale = Math::Max(ws.x, Math::Max(ws.y, ws.z));
		m_WorldDirection = rot * Configure::Vector3_DefaultForward;
		m_WorldDirection.Normalize();
		m_WorldUp = rot * Configure::Vector3_DefaultUp;
		m_WorldUp.Normalize();
		m_pLightEntity->RecaculateLight();
#ifdef _EDITOR
		m_LightGizmo->UpdateLight(this);
#endif
		return true;
	}
	return false;
}

void LightComponent::UpdateShadow()
{
	bool enableShadow = IsEnabledShadowCaster();
	if (enableShadow)
	{
		MakeSurceCreateShadowMap();
		m_pShadowMapCamera->SetActive(true);
		UpdateShadowSetting();
		UpdateLightCamera();
	}
	else
	{
		if (m_pShadowMapCamera)
		{
			m_pShadowMapCamera->SetActive(false);
		}
	}
}

void LightComponent::SetShadowType(GraphicDefine::ShadowType st)
{
	m_shadowType = st;

	if (!IsAwaked())
		return;

	CameraComponent* mainCam = m_pScene->GetMainCamera();
	if (!mainCam)
	{
		JYERROR("main camera empty!");
		return;
	}

	if ((m_eLightType == GraphicDefine::LT_DIRECTIONAL || m_eLightType == GraphicDefine::LT_SPOT))
	{
		if (st != GraphicDefine::NO_SHADOW)
		{
			if (!m_pShadowMapCamera)
			{
				InitShadowCamera();
			}
			m_pShadowMapCamera->SetActive(this->isActiveHierarchy());
		}
		else
		{
			if (m_pShadowMapCamera) m_pShadowMapCamera->SetActive(false);
		}
	}

	m_isCastShadow = st != GraphicDefine::NO_SHADOW && m_pShadowMapCamera;
}

bool LightComponent::IsEnabledShadowCaster() const
{
	return m_isCastShadow && ProjectSetting::Instance()->GetShadowRange() > 0;
}

int LightComponent::GetShadowResolution() const
{
	return 2048;
}

const Math::Mat4& LightComponent::GetLightCameraViewMatrix() const
{
	return m_pShadowMapCamera->GetView();
}

const Math::Mat4& LightComponent::GetLightCameraProjMatrix() const
{
	return m_pShadowMapCamera->GetProject();
}

const Math::Vec3& LightComponent::GetLightCameraLineDepthParam() const
{
	return m_pShadowMapCamera->LineDepthParam();
}

const Math::Vec3& LightComponent::GetLightCameraPosition() const
{
	return m_pShadowMapCamera->GetPosition();
}

void LightComponent::_DoOnAttachNode()
{
	m_pScene = GetParentObject()->GetScene();
	if (m_pScene->GeLightQueue())
	{
		m_pScene->GeLightQueue()->AttachLightComponent(this);
	}
	else
	{
		JYERROR("ILightQueue in Scene's SceneRender is null ");
	}

	m_hMessagePosition = GetParentObject()->RegisterMessageListener(IMessage::SA_POSITION, MessageDelegateDelegate::CreateRaw(this, &LightComponent::MessageLoaclPosition));
	m_hMessageScale = GetParentObject()->RegisterMessageListener(IMessage::SA_SCALE, MessageDelegateDelegate::CreateRaw(this, &LightComponent::MessageLoaclScale));
	m_hMessageRot = GetParentObject()->RegisterMessageListener(IMessage::SA_ROTATION, MessageDelegateDelegate::CreateRaw(this, &LightComponent::MessageLoaclRotation));
	
	m_hChangeStaticID = RegisterChangeIDListener(ChangeStaticIDEventDelegate::CreateRaw(this, &LightComponent::_ChangeID));
}

void LightComponent::_DoOnDetachNode()
{
	Scene* scene = GetParentObject()->GetScene();
	if (scene->GeLightQueue())
	{
		scene->GeLightQueue()->DetachLightComponent(this);
	}

	GetParentObject()->RemoveMessageListener(IMessage::SA_POSITION, m_hMessagePosition);
	GetParentObject()->RemoveMessageListener(IMessage::SA_SCALE, m_hMessageScale);
	GetParentObject()->RemoveMessageListener(IMessage::SA_ROTATION, m_hMessageRot);

	RemoveChangeIDListener(m_hChangeStaticID);
}

void LightComponent::_OnAwake()
{
	Component::_OnAwake();
	SetShadowType(m_shadowType);
}

void LightComponent::InitShadowCamera()
{
	Scene* scene = GetParentObject()->GetScene();
	m_pCameraProxy = scene->CreateObject("Shadow" + std::to_string(GetStaticID()));
	m_pCameraProxy->SetLayer(MC_MASK_EDITOR_SCENE_LAYER);
	GObject* sceneRoot = scene->GetRootNode();
	sceneRoot->DetachNode(m_pCameraProxy);
	m_pShadowMapCamera = m_pCameraProxy->CreateComponent<CameraComponent>();
	m_pShadowMapCamera->SetActive(false);
	m_pShadowMapCamera->SetSequence(std::numeric_limits<int>::min());
	m_pShadowMapCamera->SetPipelineType(GraphicDefine::PP_DEPTH);
	m_pShadowMapCamera->SetShadowBias(GetShadowBais());
	m_pShadowMapCamera->Awake();
#ifdef _EDITOR
	m_pShadowMapCamera->SetGizmoShow(false);
#endif
}

void LightComponent::UpdateShadowSetting()
{
	if (m_pShadowMapCamera != nullptr)
	{
		m_pShadowMapCamera->SetLayerMask(GetParentObject()->GetLayer());
#ifdef _EDITOR
		m_pShadowMapCamera->AddLayerMask(MC_MASK_EDITOR_SCENE_LAYER);
#endif
	}
	else
	{
		JYERROR("shadow cam not found");
	}
}

#define RAD_MAX        175
#define CORNER_COUNT   8
#define BLANK_IMAGE_SIZE 8

void InitCullData(const std::vector<Math::Vec3>& frustum, const Math::AABB& sceneBox, Math::Plane* planes,
	Math::Vec3* pVector3, unsigned char* pCount)
{
	Math::Vec3 noraml;
	float distance;

	noraml = Math::Vec3(0.0, 1.0, 0.0);
	distance = -sceneBox.GetMin().y;
	planes[0].SetNormalDistance(noraml, distance);

	noraml = Math::Vec3(0.0, -1.0, 0.0);
	distance = sceneBox.GetMax().y;
	planes[1].SetNormalDistance(noraml, distance);

	noraml = Math::Vec3(1.0, 0.0, 0.0);
	distance = -sceneBox.GetMin().x;
	planes[2].SetNormalDistance(noraml, distance);

	noraml = Math::Vec3(-1.0, 0.0, 0.0);
	distance = sceneBox.GetMax().x;
	planes[3].SetNormalDistance(noraml, distance);

	noraml = Math::Vec3(0.0, 0.0, 1.0);
	distance = -sceneBox.GetMin().z;
	planes[4].SetNormalDistance(noraml, distance);

	noraml = Math::Vec3(0.0, 0.0, -1.0);
	distance = sceneBox.GetMax().z;
	planes[5].SetNormalDistance(noraml, distance);

	pCount[0] = pCount[1] = pCount[2] = pCount[3] = pCount[4] = pCount[5] = 4;

	pVector3[0] = frustum[0];
	pVector3[1] = frustum[1];
	pVector3[2] = frustum[2];
	pVector3[3] = frustum[3];

	pVector3[4] = frustum[7];
	pVector3[5] = frustum[6];
	pVector3[6] = frustum[5];
	pVector3[7] = frustum[4];

	pVector3[8] = frustum[0];
	pVector3[9] = frustum[3];
	pVector3[10] = frustum[7];
	pVector3[11] = frustum[4];

	pVector3[12] = frustum[1];
	pVector3[13] = frustum[5];
	pVector3[14] = frustum[6];
	pVector3[15] = frustum[2];

	pVector3[16] = frustum[4];
	pVector3[17] = frustum[5];
	pVector3[18] = frustum[1];
	pVector3[19] = frustum[0];

	pVector3[20] = frustum[6];
	pVector3[21] = frustum[7];
	pVector3[22] = frustum[3];
	pVector3[23] = frustum[2];
}

void CaulteCulledPoints(std::vector<Math::Vec3>& frustum, Math::Vec3& lightDir, const Math::AABB& sceneBox, std::vector<Math::Vec3>& points)
{
	points.clear();
	int i;
	Math::Vec3 tmpPoints[3][300];
	unsigned char tmpCounts[3][150];

	Math::Vec3* pData[2] = { (Math::Vec3*)&tmpPoints[0][0], (Math::Vec3*)&tmpPoints[1][0] };
	unsigned char* pDataCounts[2] = { (unsigned char*)&tmpCounts[0][0], (unsigned char*)&tmpCounts[1][0] };
	Math::Vec3* intermPoints = &tmpPoints[2][0];
	unsigned char* intermCounts = &tmpCounts[2][0];
	unsigned int numTotalPoints;
	unsigned int swapIndex = 0;

	unsigned int numFaces = 6;

	Math::Plane planes[6];
	unsigned char* pCount = *pDataCounts;
	Math::Vec3* pVector3 = *pData;

	InitCullData(frustum, sceneBox, planes, pVector3, pCount);

	for (int p = 0; p < 6; p++)
	{
		const Math::Vec3* inputPoints = pData[swapIndex];
		Math::Vec3* outputPoints = pData[1 - swapIndex];

		unsigned char* inputCounts = pDataCounts[swapIndex];
		unsigned char* outputCounts = pDataCounts[1 - swapIndex];

		numTotalPoints = 0;
		*intermCounts = 0;

		unsigned int faceCount = numFaces;
		for (int i = 0; i < faceCount; i++)
		{
			const unsigned char numInputPoints = *inputCounts;
			if (planes[p].ClipPoly(inputPoints, numInputPoints, outputPoints,
				outputCounts, intermPoints, intermCounts))
			{
				unsigned char outputCount = *outputCounts++;
				numTotalPoints += outputCount;
				outputPoints += outputCount;
			}
			else
			{
				if (0 == (--numFaces))
				{
					break;
				}
			}

			inputCounts++;
			inputPoints += numInputPoints;
		}

		swapIndex = 1 - swapIndex;

		unsigned char numIntermPoints = *intermCounts;
		if (numIntermPoints && (p < 5))
		{
			numFaces++;
			*outputCounts = numIntermPoints;
			// error
			for (int j = 0; j < numIntermPoints; j++)
			{
				outputPoints[j] = intermPoints[j];
			}
		}
	}

	if (numFaces)
	{
		Math::Vec3 pt;
		Math::Vec3 ld = -lightDir;
		points.reserve(numTotalPoints << 1);
		Math::Vec3* inputPoints = pData[swapIndex];
		unsigned char* inputCounts = pDataCounts[swapIndex];

		for (int i = 0; i < numFaces; i++)
		{
			unsigned char numPoints = *inputCounts++;

			for (unsigned char k = 0; k < numPoints; k++)
			{
				const Math::Vec3& v = inputPoints[k];
				points.push_back(v);

				Math::Ray ray(v, ld);
				Math::Vec3 intectPoint;
				bool isInster = Math::MathUtil::intersects(ray, sceneBox, intectPoint);
				if (isInster)
				{
					points.push_back(intectPoint);
				}
			}
			inputPoints += numPoints;
		}
	}
}

void LightComponent::UpdateLightCamera()
{
	if (!m_pShadowMapCamera || !m_pScene->GetMainCamera())
		return;

	m_pShadowMapCamera->SetShadowBias(GetShadowBais());

	Math::Vec3 pos, dir;

	//场景包围盒
	std::vector<Math::Vec3> sceneBoundCornerList; sceneBoundCornerList.reserve(CORNER_COUNT);
	Math::AABB scenBound = m_pScene->GetRenderQueue()->GetBoundBox();
	sceneBoundCornerList.emplace_back(scenBound.GetMin());
	sceneBoundCornerList.emplace_back(Math::Vec3(scenBound.GetMax()));
	sceneBoundCornerList.emplace_back(Math::Vec3(scenBound.GetMax().x, scenBound.GetMin().y, scenBound.GetMin().z));
	sceneBoundCornerList.emplace_back(Math::Vec3(scenBound.GetMax().x, scenBound.GetMax().y, scenBound.GetMin().z));
	sceneBoundCornerList.emplace_back(Math::Vec3(scenBound.GetMin().x, scenBound.GetMax().y, scenBound.GetMin().z));
	sceneBoundCornerList.emplace_back(Math::Vec3(scenBound.GetMin().x, scenBound.GetMin().y, scenBound.GetMax().z));
	sceneBoundCornerList.emplace_back(Math::Vec3(scenBound.GetMax().x, scenBound.GetMin().y, scenBound.GetMax().z));
	sceneBoundCornerList.emplace_back(Math::Vec3(scenBound.GetMin().x, scenBound.GetMax().y, scenBound.GetMax().z));

	Math::Vec3 lightZ = m_WorldDirection;
	lightZ.Normalize();
	Math::Vec3 lightY = Math::Vec3::UNIT_Y;
	if (abs(lightY.Dot(lightZ)) >= 1.0f)  //up和dir重合
	{
		lightY = Math::Vec3::UNIT_Z;
	}
	Math::Vec3 lightX = lightY.Cross(lightZ);
	lightX.Normalize();
	lightY = lightZ.Cross(lightX);
	lightY.Normalize();
	Math::Mat3 matL2WRot(lightX.x, lightX.y, lightX.z,
		lightY.x, lightY.y, lightY.z,
		lightZ.x, lightZ.y, lightZ.z);

	m_pShadowMapCamera->SetShadowBias(GetShadowBais());

	if (m_eLightType == GraphicDefine::LT_DIRECTIONAL)
	{
		//主相机视锥体脚点坐标
		std::vector<Math::Vec3> cornerList;
		cornerList.reserve(CORNER_COUNT);
		for (int i = 0; i < Math::Frustum::FRUSTUM_CORNER_COUNT; i++)
		{
			Math::Vec3 corner = m_pScene->GetMainCamera()->GetFrustum().GetCorner((Math::Frustum::CornerName)i);
			cornerList.emplace_back(corner);
		}

		const Math::Frustum& frustum = m_pScene->GetMainCamera()->GetFrustum();
		cornerList[0] = frustum.GetCorner(Math::Frustum::FRUSTUM_CORNER_LBN);
		cornerList[1] = frustum.GetCorner(Math::Frustum::FRUSTUM_CORNER_RBN);
		cornerList[2] = frustum.GetCorner(Math::Frustum::FRUSTUM_CORNER_RTN);
		cornerList[3] = frustum.GetCorner(Math::Frustum::FRUSTUM_CORNER_LTN);

		cornerList[4] = frustum.GetCorner(Math::Frustum::FRUSTUM_CORNER_LBF);
		cornerList[5] = frustum.GetCorner(Math::Frustum::FRUSTUM_CORNER_RBF);
		cornerList[6] = frustum.GetCorner(Math::Frustum::FRUSTUM_CORNER_RTF);
		cornerList[7] = frustum.GetCorner(Math::Frustum::FRUSTUM_CORNER_LTF);

		float maxShadowDistance = ProjectSetting::Instance()->GetShadowRange();  //最大产生阴影的距离
		if (maxShadowDistance > 0.0)
		{
			float factor = std::min<float>(1.0, std::max<float>(0.0, maxShadowDistance * m_pScene->GetMainCamera()->LineDepthParam().y));

			cornerList[4] = (cornerList[4] - cornerList[0]) * factor + cornerList[0];
			cornerList[5] = (cornerList[5] - cornerList[1]) * factor + cornerList[1];
			cornerList[6] = (cornerList[6] - cornerList[2]) * factor + cornerList[2];
			cornerList[7] = (cornerList[7] - cornerList[3]) * factor + cornerList[3];
		}


		float maxScenLength = scenBound.GetDiagonalLength();

		Math::Vec3 trans = (m_pScene->GetMainCamera()->GetPosition() + (-m_WorldDirection) * maxScenLength * 1.0);

		//光源空间到世界空间
		Math::Mat4 matL2W(matL2WRot.a11, matL2WRot.a12, matL2WRot.a13, 0.0,
			matL2WRot.a21, matL2WRot.a22, matL2WRot.a23, 0.0,
			matL2WRot.a31, matL2WRot.a32, matL2WRot.a33, 0.0,
			trans.x, trans.y, trans.z, 1.0);
		//世界空间到光源空间
		Math::Mat4 matW2L = matL2W.GetInversed();

		Math::Vec3 sceneMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		Math::Vec3 sceneMax(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
		for (int i = 0; i < CORNER_COUNT; i++)
		{
			Math::Vec3 cornerLightSpace = sceneBoundCornerList[i] * matW2L;
			sceneMax.x = std::max(cornerLightSpace.x, sceneMax.x);
			sceneMax.y = std::max(cornerLightSpace.y, sceneMax.y);
			sceneMax.z = std::max(cornerLightSpace.z, sceneMax.z);
			sceneMin.x = std::min(cornerLightSpace.x, sceneMin.x);
			sceneMin.y = std::min(cornerLightSpace.y, sceneMin.y);
			sceneMin.z = std::min(cornerLightSpace.z, sceneMin.z);
		}

		//to light space
		Math::Vec3 viewMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
		Math::Vec3 viewMax(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min());

		std::vector<Math::Vec3> culledPoints;
		CaulteCulledPoints(cornerList, lightZ, scenBound, culledPoints);
		for (int i = 0; i < culledPoints.size(); i++)
		{
			Math::Vec3 cornerLightSpace = culledPoints[i] * matW2L;
			viewMax.x = std::max(cornerLightSpace.x, viewMax.x);
			viewMax.y = std::max(cornerLightSpace.y, viewMax.y);
			viewMax.z = std::max(cornerLightSpace.z, viewMax.z);
			viewMin.x = std::min(cornerLightSpace.x, viewMin.x);
			viewMin.y = std::min(cornerLightSpace.y, viewMin.y);
			viewMin.z = std::min(cornerLightSpace.z, viewMin.z);
		}

		viewMax.x = std::min(sceneMax.x, viewMax.x);
		viewMax.y = std::min(sceneMax.y, viewMax.y);
		viewMax.z = std::min(sceneMax.z, viewMax.z);
		viewMin.x = std::max(sceneMin.x, viewMin.x);
		viewMin.y = std::max(sceneMin.y, viewMin.y);
		viewMin.z = std::max(sceneMin.z, viewMin.z);

		Math::Vec3 lightSpaceCameraPos((viewMin.x + viewMax.x) / 2.0, (viewMin.y + viewMax.y) / 2.0, 0.0);
		Math::Vec3 worldSpaceCameraPos = lightSpaceCameraPos * matL2W;
		float orthWindowSize = std::max(abs(viewMax.x - viewMin.x), abs(viewMax.y - viewMin.y));

		float nearClipPlane = viewMin.z;
		float farClipPlane = viewMax.z;
		m_pShadowMapCamera->CreateOrthographiProjection(orthWindowSize, 1.0f, nearClipPlane, farClipPlane);
		m_pShadowMapCamera->LookAt(worldSpaceCameraPos, worldSpaceCameraPos + lightZ, lightY);
	}
}

void LightComponent::MakeSurceCreateShadowMap()
{
	if (m_pShadowMapCamera && m_pRenderTargetEntity == nullptr)
	{
		// calc resolution
		int shadowResoluton = 2048;// GetShadowResolution();
		Math::IntVec2 shadowResolutons(shadowResoluton, shadowResoluton);
		//m_shadowOldResolution = shadowResoluton;

		m_pRenderTargetEntity = _NEW RenderTargetEntity();
		m_pRenderTargetEntity->PushMetadata(
			RenderTargetMetadata(
				RHIDefine::RT_RENDER_TARGET_2D,
				Math::IntVec4(0, 0, shadowResolutons.x, shadowResolutons.y),
				shadowResolutons,
				Math::Vec2(1.0, 1.0)
			));

		bool supportShadowSampler = false; // IGraphicSystem::Instance()->SupportShadowSampler();

		/** The depth texture would be sampled latter, so MSAA should force to close for shadow rt.
			Because sampling on MSAA depth is not supported. */
		TextureEntity* colorTex = m_pRenderTargetEntity->MakeTextureAttachment(RHIDefine::TA_COLOR_0);
		colorTex->PushMetadata(
			TextureRenderMetadata(
				shadowResolutons,
				RHIDefine::TextureType::TEXTURE_2D,
				RHIDefine::TextureUseage::TU_STATIC,
				RHIDefine::PixelFormat::PF_R8G8B8A8,
				1,
				RHIDefine::TextureWarp::TW_CLAMP_TO_EDGE,
				RHIDefine::TextureWarp::TW_CLAMP_TO_EDGE,
				supportShadowSampler ? RHIDefine::TF_LINEAR : RHIDefine::TF_NEAREST,
				supportShadowSampler ? RHIDefine::TF_LINEAR : RHIDefine::TF_NEAREST));
		m_pShadowColorMap = m_pRenderTargetEntity->MakeTextureAttachment(RHIDefine::TA_DEPTH_STENCIL);
		m_pShadowColorMap->PushMetadata(
			TextureRenderMetadata(
				shadowResolutons,
				supportShadowSampler ? RHIDefine::SAMPLER_COMPARE_LEQUAL : RHIDefine::SAMPLER_COMPARE_NONE,
				RHIDefine::TextureType::TEXTURE_2D,
				RHIDefine::TextureUseage::TU_STATIC,
				RHIDefine::PixelFormat::PF_DEPTH24_STENCIL8,
				1,
				RHIDefine::TextureWarp::TW_CLAMP_TO_EDGE,
				RHIDefine::TextureWarp::TW_CLAMP_TO_EDGE,
				supportShadowSampler ? RHIDefine::TF_LINEAR : RHIDefine::TF_NEAREST,
				supportShadowSampler ? RHIDefine::TF_LINEAR : RHIDefine::TF_NEAREST));

		m_pRenderTargetEntity->CreateResource();
		//m_pShadowMapCamera->FixedResolution(shadowResolutons);
		m_pShadowMapCamera->AttachRenderTarget(m_pRenderTargetEntity);
	}
}

void LightComponent::_ChangeID(Animatable* self, uint64 oldid, uint64 newid)
{
	JY_ASSERT(self == this);
	// light组件修改id
	ILightQueue* lightqueue = GetParentObject()->GetScene()->GeLightQueue();
	auto& comMap = lightqueue->m_pAttachedComponentMap;
	auto it = comMap.find(oldid);
	if (it != comMap.end())
	{
		comMap.erase(it);
		comMap[newid] = this;
	}
}
#ifdef _EDITOR
void LightComponent::SetLightShow(bool show)
{
	m_LightGizmo->SetLightShow(show);
}
void LightComponent::CreateLightResource()
{
	m_LightGizmo->CreateLight(this);
}
bool LightComponent::GetGizmoActive()
{
	return m_LightGizmo->GetGizmoActive();
}
void LightComponent::SetGizmoActive(bool isActive)
{
	m_LightGizmo->SetGizmoActive(isActive);
}
#endif
NS_JYE_END