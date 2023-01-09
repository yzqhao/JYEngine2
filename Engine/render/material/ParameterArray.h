
#pragma once

#include "MaterialParameter.h"
#include "RHI/RHIDefine.h"
#include "math/color.h"

#include <vector>
#include <map>

NS_JYE_BEGIN

class ENGINE_API ParameterArray : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
private:
	typedef Vector<MaterialParameterPtr> ObjectArray;
	typedef HashMap<RHIDefine::ParameterSlot, unsigned>	IndexMapping;
	ObjectArray				m_ObjectArray;
	IndexMapping			m_IndexMapping;
public:
	ParameterArray(void);
	~ParameterArray(void);
	
public:
	unsigned GetParameterIndex(RHIDefine::ParameterSlot att);
	MaterialParameter* GetParameter(RHIDefine::ParameterSlot att) const;//取得用户参数
	bool hasParameter(RHIDefine::ParameterSlot att);
	void SetParameter(RHIDefine::ParameterSlot att, MaterialParameter* obj);
	void EraseParameter(RHIDefine::ParameterSlot att);
	const Vector<RHIDefine::ParameterSlot> GetAllSlots();
private:
		
public:
	inline MaterialParameter* At(unsigned index)
	{
		return m_ObjectArray[index];
	}
	inline void Rawset(unsigned index, MaterialParameter* obj)
	{
		m_ObjectArray[index] = obj;
	}
};
DECLARE_Ptr(ParameterArray);
TYPE_MARCO(ParameterArray);

NS_JYE_END
