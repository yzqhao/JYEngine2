
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"
#include "Engine/GraphicDefine.h"

NS_JYE_BEGIN

class LightComponent;

class MCmdDeserializePrefab : public MCommandBase
{
public:
	MCmdDeserializePrefab(uint64 id, const String& prefabpath);
	virtual ~MCmdDeserializePrefab();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	uint64 m_goID;
	uint64 m_prefabID;
	String m_prefabPath;
};
DECLARE_Ptr(MCmdDeserializePrefab);
DEFINE_MCMD_RETURN_TYPE(MCmdDeserializePrefab, GObject*);

NS_JYE_END