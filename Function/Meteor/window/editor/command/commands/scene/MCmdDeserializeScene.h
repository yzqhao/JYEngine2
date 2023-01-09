
#pragma once

#include "private/Define.h"
#include "../../MCommandBase.h"

NS_JYE_BEGIN

class MCmdDeserializeScene : public MCommandBase
{
public:
	MCmdDeserializeScene(const String& scenefile);
	virtual ~MCmdDeserializeScene();

	virtual Object* _DoIt();
	virtual void _Undo();
		 
private:
	Scene* m_scene;
	String m_scenefile;
};
DECLARE_Ptr(MCmdDeserializeScene);
DEFINE_MCMD_RETURN_TYPE(MCmdDeserializeScene, Scene*);

NS_JYE_END