
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdCloneNode : public MCommandBase
{
public:
	MCmdCloneNode(int64 id);
	virtual ~MCmdCloneNode();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	uint64 m_goID;
	uint64 m_cloneGoID;
};
DECLARE_Ptr(MCmdCloneNode);
DEFINE_MCMD_RETURN_TYPE(MCmdCloneNode, GObject*);

NS_JYE_END