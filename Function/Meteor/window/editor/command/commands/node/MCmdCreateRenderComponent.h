
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"

NS_JYE_BEGIN

class MCmdCreateRenderComponent : public MCommandBase
{
public:
	MCmdCreateRenderComponent(int64 id, const String& comname);
	virtual ~MCmdCreateRenderComponent();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	int64 m_comID;
	String m_compName;
};
DECLARE_Ptr(MCmdCreateRenderComponent);
DEFINE_MCMD_RETURN_TYPE(MCmdCreateRenderComponent, Component*);

NS_JYE_END