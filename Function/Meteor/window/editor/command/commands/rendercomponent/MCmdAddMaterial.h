
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdAddMaterial : public MCommandBase
{
public:
	MCmdAddMaterial(int64 id, const String& matpath, int idx);
	virtual ~MCmdAddMaterial();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	String m_matpath;
	int m_idx;
};
DECLARE_Ptr(MCmdAddMaterial);
DEFINE_MCMD_RETURN_TYPE(MCmdAddMaterial, Component*);

NS_JYE_END