
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"

NS_JYE_BEGIN

class MProjectManager;

class MInputBox
{
public:
	DECLARE_DELEGATE_RET(CallFunc, bool, const String&, String&);
public:
	MInputBox(const String& title, const String& txt, CallFunc& func);
	~MInputBox();

	void Open();
	void Draw();
	void Close();

private:

	bool m_open;
	String m_title;
	String m_txt;
	String m_tip;
	Vector<String> m_searchScenes;
	CallFunc m_callFunc;
};

NS_JYE_END