
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdAddRenderProperty : public MCommandBase
{
public:
	MCmdAddRenderProperty(int64 id, GraphicDefine::RenderProperty rp);
	virtual ~MCmdAddRenderProperty();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	GraphicDefine::RenderProperty m_rp;
};
DECLARE_Ptr(MCmdAddRenderProperty);
DEFINE_MCMD_RETURN_TYPE(MCmdAddRenderProperty, Component*);

NS_JYE_END