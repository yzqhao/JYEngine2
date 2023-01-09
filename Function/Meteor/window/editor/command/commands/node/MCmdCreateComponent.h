
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"

NS_JYE_BEGIN

class MCmdCreateComponent : public MCommandBase
{
public:
	MCmdCreateComponent(int64 id, const String& comname);
	virtual ~MCmdCreateComponent();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	int64 m_comID;
	String m_compName;
};
DECLARE_Ptr(MCmdCreateComponent);
DEFINE_MCMD_RETURN_TYPE(MCmdCreateComponent, Component*);

NS_JYE_END