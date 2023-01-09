
#pragma once

#include "window/MWindowBase.h"

NS_JYE_BEGIN

class MMainmenu;

class MEditor : public MWindowBase
{
public:
	MEditor();
	virtual ~MEditor();

	virtual void Show();
	virtual MWindowIndex Update(float dt);
	virtual void Close();

	void ShowMainWindows();

private:

	bool _Docking(float dt);

	MMainmenu* m_mainmenu;
	bool m_loadResource;
};

NS_JYE_END