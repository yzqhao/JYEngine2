#pragma once

#include "Engine/private/Define.h"
#include "Engine/render/material/BuiltinParams.h"
#include "Engine/render/material/ShaderKeyWords.h"
#include "Engine/render/RenderTargetEntity.h"
#include "Engine/GraphicDefine.h"
#include "Math/Color.h"

NS_JYE_BEGIN

struct PassInfo;
class MaterialEntity;
class ParameterArray;
class RenderObjectEntity;

struct SubMitInfo
{
	PassInfo* pShaderPass;
	RenderObjectEntity* pRenderObj;
	uint16 subMeshIdx;
};

class ENGINE_API RenderContext 
{
public:
	RenderContext();
	virtual ~RenderContext();

	FORCEINLINE ShaderKeyWords* GetShaderKeyWords()
	{
		return &m_Keywords;
	}

	FORCEINLINE BuiltinParams* GetBuiltinParams()
	{
		return m_pBuiltinParams;
	}

	FORCEINLINE RenderTargetEntity* GetCurrentRT()
	{
		return m_currentRT;
	}

	FORCEINLINE void EndRenderPass()
	{
		
	}

	FORCEINLINE bool IsInRenderPass()
	{
		return m_inRenderPass;
	}

	void BeginRenderPass(RenderTargetEntity* rt, RHIDefine::ClearFlag flag, const Math::FLinearColor& clearColor);
	void BeginRenderPass(RenderTargetEntity* rt);

	bool GenShaderPass(MaterialEntity* pMat, uint16 passIndex, ShaderKeyWords& keyWords, ParameterArray* geoParams, PassInfo& passInfo);

	void DrawShaderPass(PassInfo& passInfo, RenderObjectEntity* _ro, uint16 _subMeshIdx, uint16 instanceCount);

	void GenAndDrawShaderPass(MaterialEntity* pMat, uint16 passIndex, ShaderKeyWords& keyWords,
		ParameterArray* geoParams, RenderObjectEntity* _ro, uint16 _subMeshIdx);

	void Begin();

	void End();

protected:

	template<typename SRES>
	bool _CommitShaderParams(SRES& entity, ParameterArray* shaderParam, ParameterArray* customParam);
	bool _SubmitDraw(SubMitInfo& subMitInfo);

	FORCEINLINE void _ClearParams();

	ShaderKeyWords m_Keywords;
	BuiltinParams* m_pBuiltinParams;
	RenderTargetEntity* m_currentRT;
	bool m_inRenderPass;
};

FORCEINLINE void RenderContext::_ClearParams()
{
	m_currentRT = nullptr;
	m_inRenderPass = false;
}

NS_JYE_END