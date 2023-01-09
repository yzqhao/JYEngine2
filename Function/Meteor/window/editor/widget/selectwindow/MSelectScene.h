
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"

NS_JYE_BEGIN

class MSelectScene
{
	DECLARE_MULTICAST_DELEGATE(Callback, const String&);
public:
	MSelectScene();
	~MSelectScene();

	void Open(CallbackDelegate& func);
	void Draw();
	void Close();

private:
	bool m_open;
	String m_title;
	Vector<String> m_searchScenes;
	Callback m_callFunc;
};

NS_JYE_END