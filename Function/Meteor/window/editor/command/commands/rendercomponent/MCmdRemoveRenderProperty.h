
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdRemoveRenderProperty : public MCommandBase
{
public:
	MCmdRemoveRenderProperty(int64 id, GraphicDefine::RenderProperty rp);
	virtual ~MCmdRemoveRenderProperty();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	GraphicDefine::RenderProperty m_rp;
};
DECLARE_Ptr(MCmdRemoveRenderProperty);
DEFINE_MCMD_RETURN_TYPE(MCmdRemoveRenderProperty, Component*);

NS_JYE_END