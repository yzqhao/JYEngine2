
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdRemoveMaterial : public MCommandBase
{
public:
	MCmdRemoveMaterial(int64 id, int idx);
	virtual ~MCmdRemoveMaterial();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	String m_matpath;
	int m_idx;
};
DECLARE_Ptr(MCmdRemoveMaterial);
DEFINE_MCMD_RETURN_TYPE(MCmdRemoveMaterial, Component*);

NS_JYE_END