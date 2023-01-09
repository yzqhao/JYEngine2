
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdAttachNode : public MCommandBase
{
public:
	MCmdAttachNode(uint64 id, uint64 childid);
	virtual ~MCmdAttachNode();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	uint64 m_goID;
	uint64 m_childID;
	uint64 m_preID;
};
DECLARE_Ptr(MCmdAttachNode);
DEFINE_MCMD_RETURN_TYPE(MCmdAttachNode, GObject*);

NS_JYE_END