
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdChangeMaterial : public MCommandBase
{
public:
	MCmdChangeMaterial(int64 id, const String& matname, int matIdx);
	virtual ~MCmdChangeMaterial();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	int64 m_matIdx;
	String m_pre_matname;
	String m_matname;
};
DECLARE_Ptr(MCmdChangeMaterial);
DEFINE_MCMD_RETURN_TYPE(MCmdChangeMaterial, Component*);

NS_JYE_END