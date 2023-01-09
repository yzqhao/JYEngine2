#pragma once

#include "GeneralMetadata.h"
#include "Engine/render/material/ParameterArray.h"

NS_JYE_BEGIN

class ParameterArray;
class MaterialParameter;

class ENGINE_API MaterialStreamData : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialStreamData();
	MaterialStreamData(const String& path);
	virtual ~MaterialStreamData(void);

	FORCEINLINE const String& GetShaderPath() const;
	FORCEINLINE MaterialParameter* GetParameter(RHIDefine::ParameterSlot att) const;
	FORCEINLINE const Vector<RHIDefine::ParameterSlot> GetAllSlots();
	FORCEINLINE void SetParameter(RHIDefine::ParameterSlot slot, MaterialParameter* val);

private:
	String m_shaderPath;
	ParameterArray* m_pParameterArray;
};
DECLARE_Ptr(MaterialStreamData);
TYPE_MARCO(MaterialStreamData);

FORCEINLINE const String& MaterialStreamData::GetShaderPath() const 
{ 
	return m_shaderPath; 
}

FORCEINLINE MaterialParameter* MaterialStreamData::GetParameter(RHIDefine::ParameterSlot att) const 
{ 
	return m_pParameterArray ? m_pParameterArray->GetParameter(att) : nullptr;
}

FORCEINLINE const Vector<RHIDefine::ParameterSlot> MaterialStreamData::GetAllSlots() 
{ 
	static const Vector<RHIDefine::ParameterSlot> g_temp;
	return m_pParameterArray ? m_pParameterArray->GetAllSlots() : g_temp;
}

FORCEINLINE void MaterialStreamData::SetParameter(RHIDefine::ParameterSlot slot, MaterialParameter* val) 
{ 
	if (m_pParameterArray)
	{
		m_pParameterArray->SetParameter(slot, val);
	}
}

NS_JYE_END