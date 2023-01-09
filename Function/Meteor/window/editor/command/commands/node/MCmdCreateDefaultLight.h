
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdCreateDefaultLight : public MCommandBase
{
public:
	MCmdCreateDefaultLight(int64 id, GraphicDefine::LightType lt);
	virtual ~MCmdCreateDefaultLight();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	int64 m_comID;
	GraphicDefine::LightType m_lighttype;
};
DECLARE_Ptr(MCmdCreateDefaultLight);
DEFINE_MCMD_RETURN_TYPE(MCmdCreateDefaultLight, LightComponent*);

NS_JYE_END