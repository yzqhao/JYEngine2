
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"
#include "Engine/resource/RenderObjectMetadata.h"

NS_JYE_BEGIN

class MCmdChangeMesh : public MCommandBase
{
public:
	MCmdChangeMesh(int64 id, const RenderObjectMeshFileMetadate& meta);
	virtual ~MCmdChangeMesh();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	int64 m_goID;
	int64 m_comID;
	RenderObjectMeshFileMetadate m_meta;
	RenderObjectMeshFileMetadate m_premeta;
};
DECLARE_Ptr(MCmdChangeMesh);
DEFINE_MCMD_RETURN_TYPE(MCmdChangeMesh, Component*);

NS_JYE_END