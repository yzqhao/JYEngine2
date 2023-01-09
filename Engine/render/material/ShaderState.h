
#pragma once

#include "Engine/private/Define.h"
#include "JsonMaterialParse.h"
#include "RHI/RHI.h"
#include "ShaderResource.h"
#include "ShaderPass.h"
#include "ShaderData.h"
#include "ParameterArray.h"
#include "Math/3DMath.h"

NS_JYE_BEGIN

class ShaderData;

struct PassInfo
{
	RenderState* pRenderState;
	ShaderResource* pShaderEntity;
	ParameterArray* pParams;
	ParameterArray* pGemParams;		// Instance 参数
	bool m_useMaterailState = false;//是否使用材质里的状态
};

class ENGINE_API ShaderState
{
public:
	ShaderState();
	virtual ~ShaderState();
	ShaderState(const ShaderState& rhs);

public:
	void CreateResource(const String& shaderfile);

	void SetContent();

	ShaderPass* GetShaderPassByIndex(int passIndex) const;
	bool GetRenderPass(int passIndex, ShaderKeyWords& keyWords, PassInfo& passInfo);
	const DefProperty& GetProperty(RHIDefine::ParameterSlot slot);
	bool HasDefProperty(RHIDefine::ParameterSlot slot) const;

	bool IsSupportPass(RHIDefine::PassType passType) const;
	ShaderPass* GetShaderPass(int passType) const;

public:
	FORCEINLINE ShaderData* GetShaderData() const
	{
		return m_pShaderData;
	}
	FORCEINLINE uint GetRenderQueue() const
	{
		return m_pShaderData->renderQueue;
	}
	FORCEINLINE const String& GetMaterialName() const
	{
		return m_pShaderData->shaderName;
	}
	FORCEINLINE const String& GetShaderPath() const
	{
		return m_pShaderData->oriShaderpPath;
	}
	FORCEINLINE const Vector<DefProperty>& GetDefPropertys() const
	{
		return m_pShaderData->defPropertys;
	}
	FORCEINLINE const Vector<ShaderPass>& GetPasses() const
	{
		return m_pShaderData->shaderPassData;
	}
	FORCEINLINE uint64 GetShaderID() const
	{
		return Math::MathInstance::GetMathInstance().CRC32Compute(m_pShaderData->shaderPath.data(), m_pShaderData->shaderPath.size()) ;
	}
	FORCEINLINE uint GetBaseLightMask() const
	{
		return m_baseLightMask;
	}
private:
	Pointer<ShaderData> m_pShaderData;
	Map<RHIDefine::ParameterSlot, size_t> m_propertymap;

	uint64 m_passMask;
	uint m_baseLightMask;
	bool m_deferredMaterial;
	bool m_gpuInstancing;
};

inline bool ShaderState::IsSupportPass(RHIDefine::PassType passType) const
{
	return (m_passMask & (1ULL << passType)) != 0;
}

NS_JYE_END
