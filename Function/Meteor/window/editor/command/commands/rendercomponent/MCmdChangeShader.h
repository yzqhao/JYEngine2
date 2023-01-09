
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdChangeShader : public MCommandBase
{
public:
	MCmdChangeShader(int64 id, const String& shaderpath, int matIdx);
	virtual ~MCmdChangeShader();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	int64 m_matIdx;
	String m_pre_shaderpath;
	String m_shaderpath;
};
DECLARE_Ptr(MCmdChangeShader);
DEFINE_MCMD_RETURN_TYPE(MCmdChangeShader, Component*);

NS_JYE_END