
#pragma once

#include "private/Define.h"

NS_JYE_BEGIN

enum MWindowIndex
{
	MWI_Project = 0,
	MWI_Editor,
	MWI_Count,
};

class MWindowBase
{
public:
	MWindowBase();
	virtual ~MWindowBase();

	virtual void Show() = 0;
	virtual MWindowIndex Update(float dt) = 0;
	virtual void Close () = 0;

private:

};

NS_JYE_END