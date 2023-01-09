#include "RenderUntil.h"
#include "PipelineContex.h"
#include "Engine/render/material/MaterialEntity.h"
#include "Engine/render/material/MaterialParameter.h"
#include "Engine/render/material/ApplyKeyWord.h"
#include "Engine/render/RenderTargetEntity.h"
#include "Engine/render/RenderObjectEntity.h"
#include "Engine/component/light/LightComponent.h"
#include "Engine/component/camera/CameraComponent.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/object/GObject.h"
#include "Engine/resource/TextureMetadata.h"
#include "Engine/resource/RenderTargetMetadata.h"
#include "RenderContext.h"
#include "RHI/RHI.h"
#include "Core/Interface/ITimeSystem.h"
#include "InstanceBatcher.h"
#include "Core/Configure.h"
#include "Engine/ProjectSetting.h"

NS_JYE_BEGIN

void RenderUntil::SortShadowPassData(RenderCommandData& data)
{
	std::vector<RenderPassData>& renderPasses = data.renderPassData;
	std::vector<RenderMatData>& matDatas = data.matData;

	std::sort(renderPasses.begin(), renderPasses.end(),
		[&](const RenderPassData& ra, const RenderPassData& rb) -> bool
		{
			const RenderMatData& leftParam = matDatas[ra.matDataIndex];
			const RenderMatData& rightParam = matDatas[rb.matDataIndex];

			if (leftParam.materialIndex != rightParam.materialIndex)
			{
				return leftParam.materialIndex < rightParam.materialIndex;
			}

			uint64 leftShaderID = leftParam.mat->GetShaderID();
			uint64 rightShaderID = rightParam.mat->GetShaderID();

			if (leftShaderID != rightShaderID)
			{
				return leftShaderID < rightShaderID;
			}

			bool bEqual = Math::Equals(leftParam.distance, rightParam.distance);
			if (!bEqual)
			{
				return leftParam.distance > rightParam.distance;
			}

			return ra.matDataIndex < rb.matDataIndex;

		});
}

void RenderUntil::CollectShadowPassData(PipelineContex& pipeContext, RenderCommandData& data)
{
	const GraphicDefine::VisableRender& renders = *pipeContext.visableRender;

	//go through all visible render components
	for (int renderIndex = 0; renderIndex < renders.size(); renderIndex++)
	{
		RenderComponent* renderComponent = renders[renderIndex];
		int materialCnt = renderComponent->GetMaterialCount();
		RenderObjectEntity* pRenderObject = renderComponent->GetRenderObjectEntity();

		for (int matIndex = 0; matIndex < materialCnt; matIndex++)
		{
			MaterialEntity* pMatEntity = renderComponent->GetMaterialEntity(matIndex);
			if (pMatEntity != nullptr && pMatEntity->isReady())
			{
				uint renderQueue = pMatEntity->GetRenderQueue();
				bool casterShadow = renderComponent->isRenderProperty(GraphicDefine::RP_SHADOW_CASTER);
				bool shadowCaster = pMatEntity->IsSupportPass(RHIDefine::PT_DEPTHPASS);
				uint nDraws = pRenderObject->GetSubMeshsSize();
				unsigned short subMeshIndex = (nDraws > 0 ? matIndex % nDraws : 0);

				if (casterShadow && shadowCaster)
				{
					bool subMeshEnable = pRenderObject->GetSubMeshByIndex(subMeshIndex)->isEnable();
					if (subMeshEnable)
					{
						data.matData.emplace_back();
						RenderMatData& curRenderObj = data.matData.back();

						curRenderObj.robj = pRenderObject;
						curRenderObj.mat = pMatEntity;
						curRenderObj.renderComponent = renderComponent;
						curRenderObj.materialIndex = matIndex;
						uint nDraws = pRenderObject->GetSubMeshsSize();
						curRenderObj.submeshIndex = nDraws > 0 ? curRenderObj.materialIndex % nDraws : 0;
						curRenderObj.renderOrder = renderComponent->GetRenderOrder();
						curRenderObj.renderQueue = renderQueue;

						float sortDistance = (pipeContext.camera->GetPosition() - renderComponent->GetBindBox().GetCenter()).LengthSquared();
						curRenderObj.distance = -sortDistance;
						curRenderObj.forwardLight = nullptr;
					}
				}
			}
		}
	}
}

void RenderUntil::ComputeKeyWords(const LightComponent* lightCom, const RenderComponent* renderCom,
	ShaderKeyWords* keyWords)
{
	using namespace Configure;
	if (renderCom->isRenderProperty(GraphicDefine::RP_SKINANI))
	{
		keyWords->Enable(gpuskinKeyWord);
	}

	if (lightCom != nullptr)
	{
		keyWords->Enable(lightKeyWords[lightCom->GetLightType()]);
#if 0
		if (renderCom->isRenderProperty(GraphicDefine::RP_SHADOW_RECEIVER)
			&& lightCom->IsEnabledShadowCaster())
		{
			keyWords->Enable(shadowOnKeyWord);

			LightComponent::SoftShadowType sst = lightCom->GetSoftShadowType();
			bool supportSampler = IGraphicSystem::Instance()->SupportShadowSampler();

			if (supportSampler == false
				&& sst != LightComponent::SST_NoHardware)
			{
				sst = LightComponent::SST_NoHardware;
			}

			if (sst != LightComponent::SST_NoHardware)
			{
				keyWords->Enable(shadowHardwareKeyWord);
			}
			keyWords->Enable(softShadowType[sst - 1]);
		}
		else
		{
			keyWords->Enable(shadowOffKeyWord);
		}
#endif
	}
}

void RenderUntil::ComputeHardwareKeyWords(ShaderKeyWords* pKeyWords)
{

}

void RenderUntil::CopyRenderTarget(PipelineContex& pipeContext, RenderTargetEntity* srcRenderTarget, RenderTargetEntity* dstRenderTarget)
{
	RenderContext& pRenderContex = *pipeContext.rendercontex;
	pRenderContex.BeginRenderPass(dstRenderTarget, RHIDefine::CF_COLOR, Math::FColor(0, 0, 0, 0));

	pipeContext.pCopyMat->SetParameter(RHIDefine::PS_TEXTURE_DIFFUSE, MakeMaterialParam(srcRenderTarget->GetAttachment(RHIDefine::TA_COLOR_0)));
	RenderUntil::DrawByType(pipeContext, pipeContext.pCopyRO, pipeContext.pCopyMat, RHIDefine::PT_ALWAYS);
	pRenderContex.EndRenderPass();
}

void RenderUntil::CopyRenderTarget(PipelineContex& pipeContext, RenderTargetEntity* srcRenderTarget,
	RenderTargetEntity* dstRenderTarget, MaterialEntity* pCopymat,
	RHIDefine::ClearFlag flag, const Math::FColor& color)
{
	RenderContext& pRenderContex = *pipeContext.rendercontex;
	pRenderContex.BeginRenderPass(dstRenderTarget, flag, color);
	pCopymat->SetParameter(RHIDefine::PS_TEXTURE_DIFFUSE, MakeMaterialParam(srcRenderTarget->GetAttachment(RHIDefine::TA_COLOR_0)));
	RenderUntil::DrawByType(pipeContext, pipeContext.pCopyRO, pCopymat, RHIDefine::PT_ALWAYS);
	pRenderContex.EndRenderPass();
}

int RenderUntil::DrawPostEffects(PipelineContex& pipeContext, RenderTargetEntity* srcRenderTarget, RenderTargetEntity* dstRenderTarget)
{
	uint swtich = 0;
	bool isNeedOrig = true;
	int drawCount = 0;

	RenderTargetEntity* pCurRT = srcRenderTarget;
	RenderContext& renderContex = *pipeContext.rendercontex;

	if (pCurRT != dstRenderTarget)
	{
		CopyRenderTarget(pipeContext, pCurRT, dstRenderTarget);
		drawCount++;
	}

	return drawCount;
}

int RenderUntil::DrawObjects2RT(PipelineContex& pipeContext, RenderTargetEntity* pRenderTarget,
	RHIDefine::ClearFlag flag, const Math::FLinearColor& color, RenderCommandData** commandData, int count)
{
	int drawCount = 0;

	RenderContext& renderContex = *pipeContext.rendercontex;
	renderContex.BeginRenderPass(pRenderTarget, flag, color);

	for (int index = 0; index < count; index++)
	{
		std::vector<RenderPassData>& renderPasses = (*commandData[index]).renderPassData;
		std::vector<RenderMatData>& matDatas = (*commandData[index]).matData;

		size_t npasses = renderPasses.size();

		InstanceBatcher batch(renderContex);
		InstanceData instancData;

		for (size_t i = 0; i < npasses; i++)
		{
			RenderPassData& rpData = renderPasses[i];
			RenderMatData& robjData = matDatas[rpData.matDataIndex];
			ForwardLight* rpLight = robjData.forwardLight;

			int renderqueue = robjData.renderQueue;
			bool mulPass = ((rpData.data & MulPassTag) == MulPassTag);
			RHIDefine::PassType passType = (RHIDefine::PassType)((rpData.data >> PassTypeOffsetBits) & PassMask);
			uint16 passIdx = rpData.data & PassMask;

			switch (passType)
			{
			case RHIDefine::PT_FORWARDBASE:
			{
				ShaderKeyWords keyWords;
				LightComponent* mainLight = rpLight ? rpLight->mainLight : nullptr;
				RenderUntil::SetLightParam(pipeContext, mainLight);

				RenderUntil::ComputeKeyWords(mainLight, robjData.renderComponent, &keyWords);
				ApplyKeyWord apply(renderContex.GetShaderKeyWords(), keyWords);

				instancData.geoParams = NULL;
				instancData.keyWords = *renderContex.GetShaderKeyWords();
				instancData.passIndex = passIdx;
				instancData.pMat = robjData.mat;
				instancData.renderCompoent = robjData.renderComponent;
				instancData.renderObj = robjData.robj;
				instancData.subMeshIdx = robjData.submeshIndex;

				batch.AddBatch(instancData);

				drawCount++;
				break;
			}
			case RHIDefine::PT_FORWARDADD:
			{
				LightComponent** addLights = rpLight->GetLights();
				for (int i = 0; i < rpLight->addLightCount; i++)
				{
					ShaderKeyWords keyWords;
					LightComponent* curLight = const_cast<LightComponent*>(addLights[i]);
					RenderUntil::SetLightParam(pipeContext, curLight);
					RenderUntil::ComputeKeyWords(curLight, robjData.renderComponent, &keyWords);
					ApplyKeyWord apply(renderContex.GetShaderKeyWords(), keyWords);

					instancData.geoParams = NULL;
					instancData.keyWords = *renderContex.GetShaderKeyWords();
					instancData.passIndex = passIdx;
					instancData.pMat = robjData.mat;
					instancData.renderCompoent = robjData.renderComponent;
					instancData.renderObj = robjData.robj;
					instancData.subMeshIdx = robjData.submeshIndex;

					batch.AddBatch(instancData);
					drawCount++;
				}
				break;
			}
			case RHIDefine::PT_ALWAYS:
			case RHIDefine::PT_DEPTHPASS:
			case RHIDefine::PT_GBUFFER:
			case RHIDefine::PT_UNIVERSAL_POST_EFFECT:
			{
				ShaderKeyWords keyWords;
				RenderUntil::ComputeKeyWords(nullptr, robjData.renderComponent, &keyWords);
				ApplyKeyWord apply(renderContex.GetShaderKeyWords(), keyWords);
				RenderUntil::SetNodeParam(pipeContext, robjData.renderComponent, robjData.submeshIndex);

				instancData.geoParams = NULL;
				instancData.keyWords = *renderContex.GetShaderKeyWords();
				instancData.passIndex = passIdx;
				instancData.pMat = robjData.mat;
				instancData.renderCompoent = robjData.renderComponent;
				instancData.renderObj = robjData.robj;
				instancData.subMeshIdx = robjData.submeshIndex;

				batch.AddBatch(instancData);
				drawCount++;
				break;
			}
			default:
				break;
			}
		}

		batch.Flush();
	}

	renderContex.EndRenderPass();

	return drawCount;
}

void RenderUntil::CollectRenderingData(PipelineContex& pipeContex, bool ForceForwardRendering,
	RenderCommandData& backCommand,
	RenderCommandData& translucentData,
	RenderCommandData& overlayerData,
	RenderCommandData& forwardOpaqueData,
	RenderCommandData& deferredOpaqueData)
{
	const int supportMaxAddLightCount = 5;
	std::vector<int> forwardLightsOffset;
	std::vector<SortLight> sortLights;

	const GraphicDefine::VisableLight& lights = *pipeContex.visableLight;
	const GraphicDefine::VisableRender& renders = *pipeContex.visableRender;

	std::vector<LightComponent*> cullLights;
	cullLights.reserve(lights.size());
	for (int i = 0; i < lights.size(); i++)
	{
		cullLights.push_back(lights[i]);
	}

	// 查找每个物体受几个灯光影响
	const int LightDataSizePerNode = sizeof(ForwardLight) + supportMaxAddLightCount * sizeof(LightComponent*);
	const int LightMemorySize = LightDataSizePerNode * (renders.size());
	static std::vector<unsigned char> s_ForwardLights;
	s_ForwardLights.reserve(LightMemorySize);
	s_ForwardLights.assign(LightMemorySize, 0);
	forwardLightsOffset.resize(renders.size(), 0);
	sortLights.reserve(supportMaxAddLightCount * 2);

	int offset = 0;
	for (int i = 0; i < renders.size(); i++)
	{
		RenderComponent* curNode = renders[i];
		const Math::AABB& nodeBox = curNode->GetBindBox();
		int materialCnt = curNode->GetMaterialCount();
		RenderObjectEntity* pRenderObject = curNode->GetRenderObjectEntity();
		bool forwardRendering = ForceForwardRendering;

		if (forwardRendering)
		{
			forwardLightsOffset[i] = offset;
			ForwardLight* pCurLight = reinterpret_cast<ForwardLight*>(s_ForwardLights.data() + offset);
			pCurLight->addLightCount = 0;
			pCurLight->mainLight = nullptr;
			offset += sizeof(ForwardLight);

			// basepass 支持的灯光类型从底层获取, 这样做主要是为了兼容之前的shader同时支持新版的shader
			int baseLightMask = 0;
			int materialCnt = curNode->GetMaterialCount();
			for (int j = 0; j < materialCnt; j++)
			{
				const MaterialEntity* pMatEntity = curNode->GetMaterialEntity(j);
				if (pMatEntity != nullptr && pMatEntity->isReady())
				{
					baseLightMask |= pMatEntity->GetBaseLightMask();
				}
			}

			sortLights.clear();
			for (int j = 0; j < cullLights.size(); j++)
			{
				LightComponent* curLight = cullLights[j];

				if (curLight->isEffectOnRender(*pipeContex.camera, nodeBox))
				{
					sortLights.emplace_back();
					SortLight& curSortLight = sortLights.back();
					curSortLight.sortLight = curLight;
					curSortLight.lightEffect = curLight->CaculateEffective(*pipeContex.camera, nodeBox);
				}
			}

			if (sortLights.size() > 0)
			{
				std::sort(sortLights.begin(), sortLights.end());

				const int supportMaxLights = supportMaxAddLightCount;
				const int sortLightsCount = (sortLights.size() > supportMaxLights ? supportMaxLights : sortLights.size());

				for (int k = 0; k < sortLightsCount; k++)
				{
					LightComponent* curLight = sortLights[k].sortLight;
					GraphicDefine::LightType lightType = curLight->GetLightType();

					if (pCurLight->mainLight == nullptr
						&& (baseLightMask & (1 << lightType)))
					{
						pCurLight->mainLight = curLight;
					}
					else
					{
						*reinterpret_cast<LightComponent**>((&(s_ForwardLights.data()[offset]))) = curLight;
						offset += sizeof(LightComponent*);
						pCurLight->addLightCount++;
					}
				}
			}
		}

		for (int j = 0; j < materialCnt; j++)
		{
			MaterialEntity* pMatEntity = curNode->GetMaterialEntity(j);
			uint nDraws = pRenderObject->GetSubMeshsSize();
			unsigned short subMeshIndex = (nDraws > 0 ? j % nDraws : 0);

			if (pMatEntity != nullptr && pMatEntity->isReady())
			{
				bool subMeshEnable = pRenderObject->GetSubMeshByIndex(subMeshIndex)->isEnable();
				if (subMeshEnable)
				{
					uint renderQueue = pMatEntity->GetRenderQueue();
					bool isTran = false; // pMatEntity->IsTransparent(0);
					bool isOverlayer = (renderQueue >= RHIDefine::MRQ_OVERLAY);
					bool isBackground = (renderQueue >= RHIDefine::MRQ_BACKGROUND
						&& renderQueue < RHIDefine::MRQ_OPAQUE);

					RenderCommandData* pCommandData = nullptr;

					if (isBackground)
					{
						pCommandData = &backCommand;
					}
					else if (isOverlayer)
					{
						pCommandData = &overlayerData;
					}
					else if (isTran)
					{
						pCommandData = &translucentData;
					}
					else
					{
						pCommandData = (forwardRendering ? &forwardOpaqueData : &deferredOpaqueData);
					}

					std::vector<RenderMatData>& matData = pCommandData->matData;

					matData.emplace_back();
					RenderMatData& curRenderObj = matData.back();
					curRenderObj.robj = pRenderObject;
					curRenderObj.mat = pMatEntity;
					curRenderObj.renderComponent = curNode;
					curRenderObj.materialIndex = j;
					uint nDraws = pRenderObject->GetSubMeshsSize();
					curRenderObj.submeshIndex = nDraws > 0 ? curRenderObj.materialIndex % nDraws : 0;
					curRenderObj.renderOrder = curNode->GetRenderOrder();
					curRenderObj.renderQueue = renderQueue;

					// 计算排序距离
					float sortDistance;
					if (renderQueue >= RHIDefine::MRQ_OVERLAY)
					{
						sortDistance = curNode->GetBindBox().GetCenter().LengthSquared();
					}
					else
					{
						sortDistance = (pipeContex.camera->GetPosition() - curNode->GetBindBox().GetCenter()).LengthSquared();
					}
					curRenderObj.distance = -sortDistance;
					curRenderObj.forwardLight = forwardRendering ? reinterpret_cast<ForwardLight*>(s_ForwardLights.data() + forwardLightsOffset[i]) : nullptr;
				}
			}
		}
	}
}

void RenderUntil::SortLightingPassDataNonOpaque(RenderCommandData& commandData)
{
	std::vector<RenderPassData>& renderPasses = commandData.renderPassData;
	std::vector<RenderMatData>& matDatas = commandData.matData;

	std::sort(renderPasses.begin(), renderPasses.end(),
		[&](const RenderPassData& ra, const RenderPassData& rb) -> bool
		{
			const RenderMatData& leftParam = matDatas[ra.matDataIndex];
			const RenderMatData& rightParam = matDatas[rb.matDataIndex];

			if (leftParam.renderOrder != rightParam.renderOrder)
			{
				return leftParam.renderOrder < rightParam.renderOrder;
			}

			if (leftParam.renderQueue != rightParam.renderQueue)
			{
				return leftParam.renderQueue < rightParam.renderQueue;
			}

			bool bEqual = Math::Equals(leftParam.distance, rightParam.distance);
			if (!bEqual)
			{
				return leftParam.distance < rightParam.distance;
			}

			uint leftFirstPass = (ra.data & FirstPass);
			uint rightFirstPass = (rb.data & FirstPass);

			if (leftFirstPass != rightFirstPass)
			{
				return leftFirstPass > rightFirstPass;
			}

			if (leftParam.materialIndex != rightParam.materialIndex)
			{
				return leftParam.materialIndex < rightParam.materialIndex;
			}

			uint64 leftShaderID = leftParam.mat->GetShaderID();
			uint64 rightShaderID = rightParam.mat->GetShaderID();

			if (leftShaderID != rightShaderID)
			{
				return leftShaderID < rightShaderID;
			}

			uint passLeftIndex = (ra.data & PassMask);
			uint passRightIndex = (rb.data & PassMask);

			if (passLeftIndex != passRightIndex)
			{
				return passLeftIndex < passRightIndex;
			}

			return ra.matDataIndex < rb.matDataIndex;

		});
}

void RenderUntil::SortLightingPassDataOpaque(RenderCommandData& commandData)
{
	std::vector<RenderPassData>& renderPasses = commandData.renderPassData;
	std::vector<RenderMatData>& matDatas = commandData.matData;

	std::sort(renderPasses.begin(), renderPasses.end(),
		[&](const RenderPassData& ra, const RenderPassData& rb) -> bool
		{
			const RenderMatData& leftParam = matDatas[ra.matDataIndex];
			const RenderMatData& rightParam = matDatas[rb.matDataIndex];

			if (leftParam.renderQueue != rightParam.renderQueue)
			{
				return leftParam.renderQueue < rightParam.renderQueue;
			}

			uint leftFirstPass = (ra.data & FirstPass);
			uint rightFirstPass = (rb.data & FirstPass);

			if (leftFirstPass != rightFirstPass)
			{
				return leftFirstPass > rightFirstPass;
			}

			if (leftParam.materialIndex != rightParam.materialIndex)
			{
				return leftParam.materialIndex < rightParam.materialIndex;
			}

			uint64 leftShaderID = leftParam.mat->GetShaderID();
			uint64 rightShaderID = rightParam.mat->GetShaderID();

			if (leftShaderID != rightShaderID)
			{
				return leftShaderID < rightShaderID;
			}

			uint passLeftIndex = (ra.data & PassMask);
			uint passRightIndex = (rb.data & PassMask);

			if (passLeftIndex != passRightIndex)
			{
				return passLeftIndex < passRightIndex;
			}

			bool bEqual = Math::Equals(leftParam.distance, rightParam.distance);
			if (!bEqual)
			{
				return leftParam.distance > rightParam.distance;
			}

			return ra.matDataIndex < rb.matDataIndex;

		});
}

void RenderUntil::SetCameraParam(PipelineContex& pipeContext)
{
	BuiltinParams* instance = pipeContext.rendercontex->GetBuiltinParams();
	CameraComponent* pCamera = pipeContext.camera;

	const Math::Vec3& camPos = pCamera->GetPosition();
	//const Math::Mat3& camRot = pCamera->GetRotation();
	const Math::Vec3& forward = pCamera->GetForward();
	const Math::Mat4& viewMat = pCamera->GetView();
	const Math::Mat4& projMat = pCamera->GetProject();
	const Math::Mat4& viewProjMat = pCamera->GetViewProj();
	Math::Mat4 viewProjMatINV = viewProjMat.GetInversed();
	const Math::IntVec2& resolution = pipeContext.resolution;
	Math::Vec2 resolutionInv(1.0f / resolution.x, 1.0f / resolution.y);
	Math::Vec2 resolutionHalfInv(2.0f / resolution.x, 2.0f / resolution.y);

	SetBuiltParam(instance, RHIDefine::PS_CAMERA_WORLDPOSITION, MakeMaterialParam(camPos));
	//SetBuiltParam(instance, RHIDefine::PS_CAMERA_WORLDROTATION, MakeMaterialParam(camRot));
	SetBuiltParam(instance, RHIDefine::PS_CAMERA_WORLDDIRECTION, MakeMaterialParam(forward));
	SetBuiltParam(instance, RHIDefine::PS_CAMERA_VIEW, MakeMaterialParam(viewMat));
	SetBuiltParam(instance, RHIDefine::PS_CAMERA_PROJECTION, MakeMaterialParam(projMat));
	SetBuiltParam(instance, RHIDefine::PS_CAMERA_VIEWPROJ, MakeMaterialParam(viewProjMat));
	SetBuiltParam(instance, RHIDefine::PS_CAMERA_VIEWPROJ_INV, MakeMaterialParam(viewProjMatINV));
	//SetBuiltParam(instance, RHIDefine::PS_CAMERA_RESOLUTION, MakeMaterialParam(resolution));
	SetBuiltParam(instance, RHIDefine::PS_CAMERA_RESOLUTION_INV, MakeMaterialParam(resolutionInv));
	SetBuiltParam(instance, RHIDefine::PS_CAMERA_RESOLUTION_HALF_INV, MakeMaterialParam(resolutionHalfInv));

	Math::Vec4 lightParam;
	lightParam.x = pCamera->GetShadowBias();
	SetBuiltParam(instance, RHIDefine::PS_LIGHT_PARAM, MakeMaterialParam(lightParam));
}

void RenderUntil::SetNodeParam(PipelineContex& pipeContext, RenderComponent* renderComponent, uint16 submeshIdx)
{
	BuiltinParams* instance = pipeContext.rendercontex->GetBuiltinParams();
	GObject* pNode = renderComponent->GetParentObject();

	Math::Mat4 identity_mat;
	Math::Mat4* local2WorldMat;
	if (false)	// skin
	{
		local2WorldMat = &identity_mat;
	}
	else
	{
		local2WorldMat = (Math::Mat4*)(pNode->Attribute<Math::Mat4>(GraphicDefine::GA_WORLD_TRANSFORM));
	}

	Math::Mat4 wvpmatrix = pipeContext.camera->GetViewProj();

	if (local2WorldMat != nullptr)
	{
		SetBuiltParam(instance, RHIDefine::PS_LOCALWORLD_TRANSFORM, MakeMaterialParam(*local2WorldMat));
		wvpmatrix = *local2WorldMat * wvpmatrix;
	}

	SetBuiltParam(instance, RHIDefine::PS_LOCALSCREEN_TRANSVIEWPROJ, MakeMaterialParam(wvpmatrix));

	Math::Mat3* local2WorldRot = (Math::Mat3*)(pNode->Attribute<Math::Mat3>(GraphicDefine::GA_NORMAL_TRANSFORM));
	if (local2WorldRot != nullptr)
	{
		SetBuiltParam(instance, RHIDefine::PS_LOCALWORLD_ROTATION, MakeMaterialParam(*local2WorldRot));
	}

	Math::Vec3* worldPos = (Math::Vec3*)(pNode->Attribute<Math::Vec3>(GraphicDefine::GA_WORLD_POSITION));
	if (worldPos != nullptr)
	{
		SetBuiltParam(instance, RHIDefine::PS_WORLD_POSITION, MakeMaterialParam(*worldPos));
	}

	Math::Vec3* worldScale = (Math::Vec3*)(pNode->Attribute<Math::Vec3>(GraphicDefine::GA_WORLD_SCALE));
	if (worldScale != nullptr)
	{
		SetBuiltParam(instance, RHIDefine::PS_WORLD_SCALE, MakeMaterialParam(*worldScale));
	}
}

void RenderUntil::SetLightParam(PipelineContex& pipeContext, const LightComponent* light)
{
	BuiltinParams* instance = pipeContext.rendercontex->GetBuiltinParams();

	if (light != nullptr)
	{
		const Math::Vec3& lightPos = light->GetWorldPosition();
		const Math::Vec3& lightDir = light->GetWorldDirection();
		const Math::Vec3& lightColor = light->GetColor();
		const Math::Vec2& lightAngle = light->GetLightAngle();
		const Math::Vec2& lightDiffInv = light->GetLightInnerDiffInv();
		float lightRangeInv = 1.0f / light->GetRange();
		const Math::Vec4& lightAtten = light->GetAttenuation();

		float lightIntensity = light->GetIntensity();
		Math::Vec3 light_intense = lightColor * lightIntensity;

		SetBuiltParam(instance, RHIDefine::PS_LIGHT_POSITION, MakeMaterialParam(lightPos));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_GIVEN_DIRECTION, MakeMaterialParam(lightDir));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_COLOR, MakeMaterialParam(light_intense));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_ANGLE, MakeMaterialParam(lightAngle));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_INNER_DIFF_INV, MakeMaterialParam(lightDiffInv));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_RANGE_INV, MakeMaterialParam(lightRangeInv));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_ATTENUATION, MakeMaterialParam(lightAtten));

		if (light->IsEnabledShadowCaster())
		{
			const Math::Mat4& lightView = light->GetLightCameraViewMatrix();
			const Math::Mat4& lightProj = light->GetLightCameraProjMatrix();
			const Math::Vec3& lightCamLinearParam = light->GetLightCameraLineDepthParam();
			const Math::Vec3& lightCamPos = light->GetLightCameraPosition();
			float shadowRange = ProjectSetting::Instance()->GetShadowRange();
			Math::Vec3 shadowRanges(shadowRange, 0.0, 0.0);

			SetBuiltParam(instance, RHIDefine::PS_LIGHT_CAMERA_VIEW, MakeMaterialParam(lightView));
			SetBuiltParam(instance, RHIDefine::PS_LIGHT_CAMERA_PROJECTION, MakeMaterialParam(lightProj));
			SetBuiltParam(instance, RHIDefine::PS_LIGHT_CAMERA_LINEARPARAM, MakeMaterialParam(lightCamLinearParam));
			SetBuiltParam(instance, RHIDefine::PS_LIGHT_CAMERA_POSITION, MakeMaterialParam(lightCamPos));
			SetBuiltParam(instance, RHIDefine::PS_CAMERA_SHADOWRANGE, MakeMaterialParam(shadowRanges));

			Math::Vec3 lightParam;
			lightParam.y = light->GetShadowStrength();
			lightParam.z = 1.0f / light->GetShadowResolution();
			SetBuiltParam(instance, RHIDefine::PS_LIGHT_PARAM, MakeMaterialParam(lightParam));

			// set shadow map
			TextureEntity* pTexture = (TextureEntity*)light->GetShadowMap();
			SetBuiltParam(instance, RHIDefine::PS_TEXTURE_SHADOW_DEPTH, MakeMaterialParam(pTexture));
		}
	}
	else
	{
		static Math::Vec3 lightPos;
		static Math::Vec3 lightDir(0.0f, 1.0f, 0.0);
		static float light_intense = 0.0f;
		static float lightAngle = 0.0f;
		static float lightDiffInv = 1.0f;
		static float lightRangeInv = 1.0f;
		static float lightAtten = 1.0f;

		SetBuiltParam(instance, RHIDefine::PS_LIGHT_POSITION, MakeMaterialParam(lightPos));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_GIVEN_DIRECTION, MakeMaterialParam(lightDir));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_COLOR, MakeMaterialParam(light_intense));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_ANGLE, MakeMaterialParam(lightAngle));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_INNER_DIFF_INV, MakeMaterialParam(lightDiffInv));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_RANGE_INV, MakeMaterialParam(lightRangeInv));
		SetBuiltParam(instance, RHIDefine::PS_LIGHT_ATTENUATION, MakeMaterialParam(lightAtten));
	}
}

void RenderUntil::SetSystemParam(PipelineContex& pipeContext)
{
	BuiltinParams* instance = pipeContext.rendercontex->GetBuiltinParams();
	float timeVal = ITimeSystem::Instance()->GetGamePlayTime();
	SetBuiltParam(instance, RHIDefine::PS_SYSTEM_TIME, MakeMaterialParam(timeVal));
}

void RenderUntil::SetAmbientLightParam(PipelineContex& pipeContext)
{
	Math::Vec3 ambientColor;
	const GraphicDefine::VisableLight& lights = *pipeContext.visableLight;

	for (int i = 0; i < lights.size(); i++)
	{
		if (GraphicDefine::LT_AMBIENT == lights[i]->GetLightType())
		{
			ambientColor += lights[i]->GetColor();
		}
	}

	BuiltinParams* instance = pipeContext.rendercontex->GetBuiltinParams();
	SetBuiltParam(instance, RHIDefine::PS_AMBIENT_COLOR, MakeMaterialParam(ambientColor));
}

void RenderUntil::DrawByType(PipelineContex& pipeContext, RenderObjectEntity* ro, MaterialEntity* mat, int passType)
{
	const std::vector<ShaderPass>& passes = mat->GetPasses();
	int passIndex = -1;
	for (int passIdx = 0; passIdx < passes.size(); ++passIdx)
	{
		const ShaderPass& curPass = passes[passIdx];
		RHIDefine::PassType curType = curPass.passType;
		if (curType == passType)
		{
			passIndex = passIdx;
			break;
		}
	}

	if (passIndex == -1)
	{
		for (int passIdx = 0; passIdx < passes.size(); ++passIdx)
		{
			const ShaderPass& curPass = passes[passIdx];
			RHIDefine::PassType curType = curPass.passType;
			if (curType == RHIDefine::PT_ALWAYS)
			{
				passIndex = passIdx;
				break;
			}
		}
	}

	bool subMeshEnable = ro->GetSubMeshByIndex(0)->isEnable();
	if (subMeshEnable)
	{
		pipeContext.rendercontex->GenAndDrawShaderPass(mat, passIndex, *(pipeContext.rendercontex->GetShaderKeyWords()), NULL, ro, 0);
	}
}

RenderTargetEntity* RenderUntil::CreateRenderTarget(const Math::IntVec2& resolution, RHIDefine::PixelFormat format, bool hasDepthStencil)
{
	RenderTargetEntity* pRenderTarget = _NEW RenderTargetEntity;

	pRenderTarget->PushMetadata(
		RenderTargetMetadata(
			RHIDefine::RT_RENDER_TARGET_2D,
			Math::IntVec4(0, 0, resolution.x, resolution.y),
			resolution,
			Math::Vec2(1.0f, 1.0f)
		));

	TextureEntity* tex = pRenderTarget->MakeTextureAttachment(RHIDefine::TA_COLOR_0);
	tex->PushMetadata(
		TextureRenderMetadata(
			resolution,
			RHIDefine::TextureType::TEXTURE_2D,
			RHIDefine::TextureUseage::TU_STATIC,
			format,
			1,
			RHIDefine::TextureWarp::TW_CLAMP_TO_EDGE,
			RHIDefine::TextureWarp::TW_CLAMP_TO_EDGE,
			RHIDefine::TextureFilter::TF_NEAREST,
			RHIDefine::TextureFilter::TF_NEAREST));

	if (hasDepthStencil)
	{
		TextureEntity* depTexEnt = pRenderTarget->MakeTextureAttachment(RHIDefine::TA_DEPTH_STENCIL);
		depTexEnt->PushMetadata(DepthRenderBufferMetadata(
			resolution,
			RHIDefine::PixelFormat::PF_DEPTH24_STENCIL8
		));
	}

	pRenderTarget->CreateResource();

	return pRenderTarget;
}

void RenderUntil::ComputeDeferredKeyWords(const LightComponent* lightCom,
	ShaderKeyWords* keyWords)
{
	
}

void RenderUntil::GenRenderPasses(RenderCommandData& commandData, int* supportedPass, int passLen)
{
	int commandCount = commandData.matData.size();

	if (commandCount)
	{
		std::vector<RenderPassData>& renderPasses = commandData.renderPassData;
		std::vector<RenderMatData>& matDatas = commandData.matData;

		uint passMask = 0;
		for (int j = 0; j < passLen; j++)
		{
			int curPass = supportedPass[j];
			if (curPass != RHIDefine::PT_FORWARDADD)
			{
				passMask |= (1 << curPass);
			}
		}

		renderPasses.reserve(commandCount * 3);

		for (int i = 0; i < commandCount; i++)
		{
			RenderMatData& curRenderData = matDatas[i];
			const std::vector<ShaderPass>& passes = curRenderData.mat->GetPasses();
			ForwardLight* forwardLight = curRenderData.forwardLight;

			int beginIndex = renderPasses.size() - 1;
			int validPassCount = 0;
			uint firstPassTag = FirstPass;
			for (int passIndex = 0; passIndex < passes.size(); passIndex++)
			{
				const ShaderPass& curPass = passes[passIndex];
				RHIDefine::PassType passType = curPass.passType;

				bool supportPass = ((1 << passType) & passMask);
				//supportPass |= (passType == PipelineProperty::PT_FORWARDADD && forwardLight != nullptr && forwardLight->addLightCount > 0);

				if (supportPass)
				{
					RenderPassData rpassData;
					rpassData.matDataIndex = i;
					rpassData.data = ((passIndex & PassMask) | (passType << PassTypeOffsetBits) |
						firstPassTag);
					validPassCount++;
					firstPassTag = 0;
					renderPasses.push_back(rpassData);
				}
			}
		}
	}
}

NS_JYE_END