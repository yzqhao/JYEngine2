#pragma once

#include "Engine/private/Define.h"
#include "Math/IntVec2.h"
#include "Math/IntVec4.h"
#include "Math/Vec2.h"
#include "Math/Vec4.h"
#include "Engine/graphic/IAsyncResource.h"
#include "RHI/RHIDefine.h"
#include "Engine/render/material/ParameterArray.h"
#include "Engine/render/material/ShaderKeyWords.h"
#include "Engine/render/material/ShaderState.h"

NS_JYE_BEGIN

class ENGINE_API MaterialResource : public ISharedResource
{
public:
	MaterialResource();
	virtual ~MaterialResource();

	const MaterialResource& operator = (const MaterialResource& rhs);
public:
	virtual void ReleaseResource();

	FORCEINLINE void EnableKeyWord(const String& key);
	FORCEINLINE void DisableKeyWord(const String& key);
	FORCEINLINE void DisableAllKeyWord();
	FORCEINLINE bool IsKeyWordEnable(const String& key);
	FORCEINLINE ShaderKeyWords GetKeyWords();
	FORCEINLINE bool hasParameter(RHIDefine::ParameterSlot att);
	FORCEINLINE const String& GetMaterialName() const;
	FORCEINLINE const String& GetShaderPath() const;
	FORCEINLINE uint GetRenderQueue() const;
	FORCEINLINE bool IsSupportPass(RHIDefine::PassType passType) const;
	FORCEINLINE ParameterArray* GetParmArray();
	FORCEINLINE const Vector<ShaderPass>& GetPasses() const;
	FORCEINLINE void SetStateEntity(ShaderState* pShaderData);
	FORCEINLINE uint64 GetShaderID() const;
	FORCEINLINE uint GetBaseLightMask() const;
	FORCEINLINE bool HasDefProperty(RHIDefine::ParameterSlot att) const;
	FORCEINLINE const DefProperty& GetProperty(RHIDefine::ParameterSlot slot);

	bool SetParameter(RHIDefine::ParameterSlot att, MaterialParameter* obj);
	bool SetParameter(const String& paramName, MaterialParameter* obj);
	MaterialParameter* GetParameter(RHIDefine::ParameterSlot att);
	MaterialParameter* GetParameter(const String& paramName);
	bool GetRenderPass(int passIndex, ShaderKeyWords& keyWords, PassInfo& passInfo);
	Map<String, MaterialParameter*> GetParameters();
	void SyncDefProperty(const HashMap<String, MaterialParameter*>* params);
private:
	void _DoCopy(const MaterialResource& rhs);

	virtual bool _DeprecatedFilter(bool isd) override;

	ParameterArray* m_pParameterArray;
	ShaderKeyWords m_KeyWords;
	Vector<String> m_KeyWordStrs;
	ShaderState* m_pShaderState;
};


FORCEINLINE void MaterialResource::EnableKeyWord(const String& strKey)
{
	m_KeyWords.Enable(strKey);
}

FORCEINLINE void MaterialResource::DisableKeyWord(const String& strKey)
{
	m_KeyWords.Disable(strKey);
}

FORCEINLINE void MaterialResource::DisableAllKeyWord()
{
	m_KeyWords.Reset();
}

FORCEINLINE bool MaterialResource::IsKeyWordEnable(const String& strKey)
{
	return m_KeyWords.IsEnabled(strKey);
}

FORCEINLINE ShaderKeyWords MaterialResource::GetKeyWords()
{
	return m_KeyWords;
}

FORCEINLINE bool MaterialResource::hasParameter(RHIDefine::ParameterSlot att)
{
	return m_pParameterArray->hasParameter(att);
}

FORCEINLINE const String& MaterialResource::GetMaterialName() const
{
	return m_pShaderState->GetMaterialName();
}

FORCEINLINE const String& MaterialResource::GetShaderPath() const
{
	return m_pShaderState->GetShaderPath();
}

FORCEINLINE uint MaterialResource::GetRenderQueue() const
{
	return m_pShaderState->GetRenderQueue();
}

FORCEINLINE bool MaterialResource::IsSupportPass(RHIDefine::PassType passType) const
{
	return m_pShaderState->IsSupportPass(passType);
}

FORCEINLINE ParameterArray* MaterialResource::GetParmArray()
{
	return m_pParameterArray;
}

FORCEINLINE const Vector<ShaderPass>& MaterialResource::GetPasses() const
{
	return m_pShaderState->GetPasses();
}

FORCEINLINE void MaterialResource::SetStateEntity(ShaderState* pShaderData)
{
	m_pShaderState = pShaderData;

	//SyncDefProperty(nullptr);
}

FORCEINLINE uint64 MaterialResource::GetShaderID() const
{
	return m_pShaderState->GetShaderID();
}

FORCEINLINE uint MaterialResource::GetBaseLightMask() const
{
	return m_pShaderState->GetBaseLightMask();
}

FORCEINLINE bool MaterialResource::HasDefProperty(RHIDefine::ParameterSlot att) const
{
	return m_pShaderState->HasDefProperty(att);
}

FORCEINLINE const DefProperty& MaterialResource::GetProperty(RHIDefine::ParameterSlot slot)
{
	return m_pShaderState->GetProperty(slot);
}

NS_JYE_END