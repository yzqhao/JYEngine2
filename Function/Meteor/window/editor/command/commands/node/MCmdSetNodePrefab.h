
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdSetNodePrefab : public MCommandBase
{
public:
	MCmdSetNodePrefab(uint64 id, const String& prefabPath);
	virtual ~MCmdSetNodePrefab();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	uint64 m_goID;
	String m_prefabPath;
};
DECLARE_Ptr(MCmdSetNodePrefab);
DEFINE_MCMD_RETURN_TYPE(MCmdSetNodePrefab, GObject*);

NS_JYE_END