
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"
#include "Engine/render/material/MaterialParameter.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdSetParameter : public MCommandBase
{
public:
	MCmdSetParameter(int64 id, int idx, RHIDefine::ParameterSlot slot, MaterialParameterPtr cur);
	virtual ~MCmdSetParameter();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	int64 m_comID;
	RHIDefine::ParameterSlot m_slot;
	int m_idx;
	MaterialParameterPtr m_pre;
	MaterialParameterPtr m_cur;
};
DECLARE_Ptr(MCmdSetParameter);
DEFINE_MCMD_RETURN_TYPE(MCmdSetParameter, Component*);

NS_JYE_END