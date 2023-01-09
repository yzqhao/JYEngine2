
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdDeleteComponent : public MCommandBase
{
public:
	MCmdDeleteComponent(int64 id, const String& comname);
	virtual ~MCmdDeleteComponent();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	int64 m_comID;
	String m_compName;
};
DECLARE_Ptr(MCmdDeleteComponent);
DEFINE_MCMD_RETURN_TYPE(MCmdDeleteComponent, Component*);

NS_JYE_END