#include "RenderContext.h"
#include "Engine/render/material/MaterialEntity.h"
#include "Engine/render/RenderObjectEntity.h"
#include "Engine/render/material/ParameterArray.h"
#include "Engine/render/material/ShaderState.h"
#include "Engine/render/material/MaterialUtil.h"
#include "ContextUntil.h"

NS_JYE_BEGIN

RenderContext::RenderContext()
	: m_currentRT(nullptr)
	, m_pBuiltinParams(nullptr)
	, m_inRenderPass(false)
{

}

RenderContext::~RenderContext()
{

}

template<typename SRES>
bool RenderContext::_CommitShaderParams(SRES& entity, ParameterArray* shaderParam, ParameterArray* customParam)
{
	UnifromFunctionList& uniformList = entity.GetUniformList();
	bool result = true;

	for (auto it = uniformList.begin(); it != uniformList.end(); it++)
	{
		result = ContextUntil::SetShaderParam(*this, **it, shaderParam, customParam);

		if (!result)
		{
			break;
		}
	}

	return result;
}

bool RenderContext::_SubmitDraw(SubMitInfo& subMitInfo)
{
	RenderObjectEntity* pMesh = subMitInfo.pRenderObj;
	SubMesh* pSubMesh = pMesh->GetSubMeshByIndex(subMitInfo.subMeshIdx);
	PassInfo& passInfo = *subMitInfo.pShaderPass;

	ParameterArray* shaderParam = passInfo.pParams;
	RenderState* rs = passInfo.pRenderState;
	MatUtil::ApplyStateVar(rs, shaderParam);
	RHISetRenderState(rs->GetRenderStateFlag());
	RHISetScissor(rs->GetScissor());

	bool result = _CommitShaderParams<ShaderResource>(*passInfo.pShaderEntity, shaderParam, subMitInfo.pShaderPass->pGemParams);

	if (result)
	{
		handle programHandle = passInfo.pShaderEntity->GetGpuResourceHandle();
		IndicesBufferEntity* indexBuffer = pMesh->GetIndexBufferEntity();
		bool hasIndexBuffer = (indexBuffer != nullptr);

		handle vertexHandle = pMesh->GetVertexBufferEntity()->GetGpuHandle();
		handle indexHandle = (hasIndexBuffer ? indexBuffer->GetGpuHandle() : nullhandle);

		RHIDefine::RenderMode renderMode = pMesh->GetRenderMode();
		const Math::IntVec2 count_offset = pSubMesh->GetDrawRange();

		if (indexBuffer != nullptr)
		{
			const Math::IntVec2 count_offset = pSubMesh->GetDrawRange();
			uint startIndex = count_offset.y;
			uint drawIndexcount = count_offset.x;

			RHISubmitIndexedDraw(renderMode, vertexHandle, indexHandle, programHandle, startIndex, drawIndexcount);
		}
		else
		{
			
		}
	}
	else
	{
		JYERROR("RenderContext::SubmitDraw has some error, maybe shader is binded failed!");
	}

	return result;
}

void RenderContext::BeginRenderPass(RenderTargetEntity* rt, RHIDefine::ClearFlag flag, const Math::FLinearColor& clearColor)
{
	rt->MakeSureCreate();
	//m_pEncoder->BeginRenderPass();
	m_currentRT = rt;

	JY_ASSERT(m_inRenderPass == false);
	m_inRenderPass = true;

	handle rtHandle = rt->GetGPUHandle();
	const Math::IntVec4& viewPort = rt->GetViewPort();
	const Math::IntVec2& resolution = rt->GetResolution();

	uint16 viewID = RHIGetUniformViewID();
	RHISetCurrentViewID(viewID);
	RHISetRenderTarget(rtHandle);
	RHISetRenderTargetSize(resolution);
	RHISetViewPort(viewPort);

	if (flag != RHIDefine::CF_None)
	{
		RHISetClear(flag, clearColor.ToFColor(false));
	}

	float yflip = RHIGetDeviceCoordinateYFlip(rtHandle);
	BuiltinParams* pBuiltin = GetBuiltinParams();
	SetBuiltParam(pBuiltin, RHIDefine::PS_DEVICE_COORDINATE_Y_FLIP, MakeMaterialParam(yflip));
}

void RenderContext::BeginRenderPass(RenderTargetEntity* rt)
{
	BeginRenderPass(rt, RHIDefine::CF_None, Math::FColor::Black);
}

bool RenderContext::GenShaderPass(MaterialEntity* pMat, uint16 passIndex, ShaderKeyWords& keyWords, ParameterArray* geoParams, PassInfo& passInfo)
{
	passInfo.pGemParams = geoParams;
	return pMat->GetRenderPass(passIndex, keyWords, passInfo);
}

void RenderContext::DrawShaderPass(PassInfo& shaderpass, RenderObjectEntity* renderObj, uint16 subMeshIdx, uint16 instanceCount)
{
	SubMitInfo subMitInfo;
	subMitInfo.pShaderPass = &shaderpass;
	subMitInfo.pRenderObj = renderObj;
	subMitInfo.subMeshIdx = subMeshIdx;

	_SubmitDraw(subMitInfo);
}

void RenderContext::GenAndDrawShaderPass(MaterialEntity* pMat, uint16 passIndex, ShaderKeyWords& keyWords,
	ParameterArray* geoParams, RenderObjectEntity* _ro, uint16 _subMeshIdx)
{
	PassInfo passInfo;
	bool result = GenShaderPass(pMat, passIndex, keyWords,
		NULL, passInfo);
	if (result)
	{
		DrawShaderPass(passInfo, _ro, _subMeshIdx, 1);
	}
}

void RenderContext::Begin()
{
	//m_pEncoder = IGraphicSystem::Instance()->BeginEncoder();
	m_pBuiltinParams = _NEW BuiltinParams();
}

void RenderContext::End()
{
	//IGraphicSystem::Instance()->EndEncoder(m_pEncoder);
	//m_ConstBuffers.FreeAll();
	SAFE_DELETE(m_pBuiltinParams);
	_ClearParams();
}

NS_JYE_END