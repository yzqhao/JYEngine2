
#pragma once

#include "private/Define.h"
#include "Core/Object.h"

NS_JYE_BEGIN

class Component;

class MPanelBase : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MPanelBase(const String& name);
	~MPanelBase();

	virtual bool OnGui(Component* com, float dt) = 0;

	FORCEINLINE const String& GetPanelName() const { return m_panelName; }
private:
	String m_panelName;
};
DECLARE_Ptr(MPanelBase);

NS_JYE_END