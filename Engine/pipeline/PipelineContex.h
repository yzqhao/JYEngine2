#pragma once

#include "Engine/private/Define.h"
#include "Math/IntVec2.h"
#include "Math/Color.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class CameraComponent;
class RenderComponent;
class MaterialEntity;
class RenderTargetEntity;
class Scene;
class RenderContext;
class LightComponent;
class RenderObjectEntity;

enum Constant
{
	FirstPass = 1 << 25,
	MulPassTag = 1 << 26,
	PassTypeOffsetBits = 8,
	PassMask = 255
};

struct ForwardLight
{
	int addLightCount;
	LightComponent* mainLight;

	LightComponent** GetLights()
	{
		return reinterpret_cast<LightComponent**>(reinterpret_cast<char*>(this) + sizeof(ForwardLight));
	}
};

struct SortLight
{
	LightComponent* sortLight;
	float lightEffect;

	friend bool operator< (const SortLight& lhs, const SortLight& rhs)
	{
		return lhs.lightEffect > rhs.lightEffect;
	}
};

//Data needed for render a RenderComponent
struct RenderMatData
{
	MaterialEntity* mat = nullptr;
	RenderComponent* renderComponent = nullptr;
	RenderObjectEntity* robj = nullptr;
	uint renderQueue;
	int renderOrder;
	unsigned short materialIndex;
	unsigned short submeshIndex;
	float distance;
	ForwardLight* forwardLight = nullptr;
};

//Render pass in RenderComponent, RenderComponent may contains more than one shader pass.
//This is the lowest abstraction of render data, one level above RHI.
struct RenderPassData
{
	//index of RenderMatData in the RenderCommandData
	uint matDataIndex;
	//bit mask of pass information.
	uint data;
};

//Contains all render data of a particular type(Background,Opaque,translucent)
//Background objects will be drawn first, then opaque objects, translucent objects last.
struct RenderCommandData
{
	void Clear()
	{
		matData.clear();
		renderPassData.clear();
	}
	std::vector<RenderMatData>  matData;
	std::vector<RenderPassData> renderPassData;
};

//Base class for the renderpipe. 
struct PipelineContex
{
	RenderTargetEntity* m_pMainRenderTarget;
	RenderTargetEntity* m_pAttacedRenderTarget;
	RenderTargetEntity* m_pLightingPassRT;
	RenderTargetEntity* m_pScriptPostEffectRT;
	RenderTargetEntity* m_pSceneRT;

	//RT for GRAP
	RenderTargetEntity* m_pCurPushRT;

	LightComponent* m_pAmbientLight;

	GraphicDefine::PiplineType m_ePipelinePass;

	Math::IntVec2 resolution;
	Scene* scene;
	RHIDefine::ClearFlag clearFlag;
	Math::FLinearColor clearColor;

	CameraComponent* camera;
	CameraComponent* hostCamera;
	RenderContext* rendercontex;
	const GraphicDefine::VisableLight* visableLight;
	const GraphicDefine::VisableRender* visableRender;
	RenderObjectEntity* pCopyRO;
	MaterialEntity* pCopyMat;
	RenderTargetEntity* pDrawRT;
	RenderTargetEntity* pMainRT;
	RenderTargetEntity* pPingPongRT[2];
};

NS_JYE_END