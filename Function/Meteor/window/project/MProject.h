
#pragma once

#include "window/MWindowBase.h"

NS_JYE_BEGIN

class MSelectScene;
class MInputBox;

class MProject : public MWindowBase
{
public:
	MProject();
	virtual ~MProject();

	virtual void Show();
	virtual MWindowIndex Update(float dt);
	virtual void Close();

	bool DrawStartPage();

private:

	MSelectScene* m_selectScene;
	MInputBox* m_createScene;
	bool m_opend;
	bool m_isOpenDialog;
};

NS_JYE_END