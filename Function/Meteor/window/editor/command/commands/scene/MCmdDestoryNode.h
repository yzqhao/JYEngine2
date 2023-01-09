
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"

NS_JYE_BEGIN

class GObject;

class MCmdDestoryNode : public MCommandBase
{
public:
	MCmdDestoryNode(uint64 gid);
	virtual ~MCmdDestoryNode();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	uint64 m_nodeID;
};
DECLARE_Ptr(MCmdDestoryNode);
DEFINE_MCMD_RETURN_TYPE(MCmdDestoryNode, GObject*);

NS_JYE_END