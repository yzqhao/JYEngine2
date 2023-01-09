
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdSerializePrefab : public MCommandBase
{
public:
	MCmdSerializePrefab(uint64 id, const String& savepath);
	virtual ~MCmdSerializePrefab();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	uint64 m_goID;
	String m_prefabPath;
};
DECLARE_Ptr(MCmdSerializePrefab);
DEFINE_MCMD_RETURN_TYPE(MCmdSerializePrefab, GObject*);

NS_JYE_END