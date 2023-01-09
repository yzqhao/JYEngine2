
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"

NS_JYE_BEGIN

class GObject;

class MCmdCreateGenericNode : public MCommandBase
{
public:
	MCmdCreateGenericNode(const String& goname);
	virtual ~MCmdCreateGenericNode();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	String m_goName;
	uint64 m_nodeID;
	uint64 m_transID;
};
DECLARE_Ptr(MCmdCreateGenericNode);
DEFINE_MCMD_RETURN_TYPE(MCmdCreateGenericNode, GObject*);

NS_JYE_END