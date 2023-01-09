#pragma once

#include "Engine/private/Define.h"
#include "RHI/RHI.h"

#include <mutex>

NS_JYE_BEGIN

class ENGINE_API IMaterialSystem
{
public:
	struct ENGINE_API InputParameter
	{
		RHIDefine::ParameterUsage		Usage;
		RHIDefine::ParameterSlot		Slot;
		String							Name;
		InputParameter(RHIDefine::ParameterUsage pu, RHIDefine::ParameterSlot ps, const String& name);
		InputParameter() = default;
	};
	typedef std::map<RHIDefine::ParameterSlot, InputParameter>	ShaderParameterMap;
	typedef std::map<String, RHIDefine::ParameterSlot>	RegisteredSlot;
public:

	IMaterialSystem();
	virtual ~IMaterialSystem();

	static IMaterialSystem* Instance();
	static void Destory();

	RHIDefine::ParameterSlot NewParameterSlot(RHIDefine::ParameterUsage pu, const String& name);

	inline void NewParameterSlot(RHIDefine::ParameterUsage pu, RHIDefine::ParameterSlot ps, const String& name);
	inline const InputParameter& GetParameterInfomation(RHIDefine::ParameterSlot ps);
	inline RHIDefine::ParameterSlot GetParameterSlot(const String& name);
	inline bool ParameterSlotExist(const String& name);
protected:
	static IMaterialSystem* s_instance;

	std::recursive_mutex						m_Mutex;

	RegisteredSlot								m_RegisteredSlot;
	ShaderParameterMap							m_ShaderParameterMap;

	int											m_AttributeIndex;
	int											m_UniformIndex;
	int											m_InternalIndex;
	int											m_UnknownIndex;
};

inline void IMaterialSystem::NewParameterSlot(RHIDefine::ParameterUsage pu, RHIDefine::ParameterSlot ps, const String& name)
{
	m_Mutex.lock();
	if (m_RegisteredSlot.end() == m_RegisteredSlot.find(name)
		&& m_ShaderParameterMap.end() == m_ShaderParameterMap.find(ps))
	{
		m_RegisteredSlot.insert({ name, ps });
		m_ShaderParameterMap.insert({ ps, InputParameter(pu, ps, name) });
	}
	else
	{
		JYLOGTAG(LogEngineRHI, "parameter name '%s' or solt '%d' already exists ", name.c_str(), ps);
	}
	m_Mutex.unlock();
}

inline const IMaterialSystem::InputParameter& IMaterialSystem::GetParameterInfomation(RHIDefine::ParameterSlot ps)
{
	m_Mutex.lock();
	auto res = m_ShaderParameterMap.find(ps);
	m_Mutex.unlock();
	return res->second;
}

inline RHIDefine::ParameterSlot IMaterialSystem::GetParameterSlot(const String& name)
{
	RHIDefine::ParameterSlot ps = RHIDefine::PS_ERRORCODE;
	m_Mutex.lock();
	RegisteredSlot::iterator it = m_RegisteredSlot.find(name);
	if (m_RegisteredSlot.end() != it)
	{
		ps = it->second;
	}
	else
	{
		JYLOGTAG(LogEngineRHI, "parameter name '%s' is not exists ", name.c_str());
	}
	m_Mutex.unlock();
	return ps;
}

inline bool IMaterialSystem::ParameterSlotExist(const String& name)
{
	bool exist = false;
	m_Mutex.lock();
	RegisteredSlot::iterator it = m_RegisteredSlot.find(name);
	if (m_RegisteredSlot.end() != it)
	{
		exist = true;
	}
	m_Mutex.unlock();
	return exist;
}

NS_JYE_END
