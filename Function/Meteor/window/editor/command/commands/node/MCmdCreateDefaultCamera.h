
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"

NS_JYE_BEGIN

class CameraComponent;

class MCmdCreateDefaultCamera : public MCommandBase
{
public:
	MCmdCreateDefaultCamera(uint64 id, bool isMain);
	virtual ~MCmdCreateDefaultCamera();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	uint64 m_goID;
	uint64 m_comID;
	bool m_isMain;
};
DECLARE_Ptr(MCmdCreateDefaultCamera);
DEFINE_MCMD_RETURN_TYPE(MCmdCreateDefaultCamera, CameraComponent*);

NS_JYE_END