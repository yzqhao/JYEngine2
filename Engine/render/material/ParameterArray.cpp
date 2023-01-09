#include "ParameterArray.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN


IMPLEMENT_RTTI(ParameterArray, Object);

IMPLEMENT_INITIAL_BEGIN(ParameterArray)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(ParameterArray, Object)
REGISTER_PROPERTY(m_ObjectArray, m_ObjectArray, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_IndexMapping, m_IndexMapping, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

ParameterArray::ParameterArray(void)
{

}

ParameterArray::~ParameterArray(void)
{

	//for (MaterialParameter* it : m_ObjectArray)
	//{
	//	SAFE_DELETE(it);
	//}
	m_ObjectArray.clear();
}

unsigned ParameterArray::GetParameterIndex(RHIDefine::ParameterSlot att)
{
	IndexMapping::iterator it = m_IndexMapping.find(att);
	if (m_IndexMapping.end() == it)
	{
		unsigned index = m_ObjectArray.size();
		m_ObjectArray.push_back(NULL);
		m_IndexMapping.insert({ att, index });
		return index;
	}
	return it->second;
}

MaterialParameter* ParameterArray::GetParameter(RHIDefine::ParameterSlot att) const
{
	IndexMapping::const_iterator it = m_IndexMapping.find(att);
	if (it == m_IndexMapping.end())
	{
		JYLOGTAG(LogEngineRHI, "GetParameter not find");
#if _DEBUG
		JY_ASSERT(false);
#else
		return nullptr;
#endif
	}
	return m_ObjectArray[it->second];
}

void ParameterArray::SetParameter(RHIDefine::ParameterSlot att, MaterialParameter* obj)
{
	uint index = GetParameterIndex(att);
	m_ObjectArray[index] = obj;
}

void ParameterArray::EraseParameter(RHIDefine::ParameterSlot att)
{
	IndexMapping::const_iterator it = m_IndexMapping.find(att);
	if (m_IndexMapping.end() != it)
	{
		m_ObjectArray[it->second] = nullptr;
	}
}

const Vector<RHIDefine::ParameterSlot> ParameterArray::GetAllSlots()
{
	Vector<RHIDefine::ParameterSlot> slots;
	for(auto& it : m_IndexMapping) 
	{
		slots.push_back(it.first);
	}
	return std::move(slots);
}

bool ParameterArray::hasParameter(RHIDefine::ParameterSlot att)
{
	return m_IndexMapping.end() != m_IndexMapping.find(att);
}

NS_JYE_END
